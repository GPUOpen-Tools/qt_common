//=============================================================================
/// Copyright (c) 2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a navigation list view
//=============================================================================

#include "navigation_list_view.h"

#include <QDebug>
#include <QMouseEvent>
#include <QFont>

#include "utils/scaling_manager.h"

NavigationListView::NavigationListView(QWidget* parent)
    : QListView(parent)
{
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    setStyleSheet(kMainListWidgetStylesheet_);

    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &NavigationListView::OnScaleFactorChanged);
}

NavigationListView::~NavigationListView()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &NavigationListView::OnScaleFactorChanged);
}

void NavigationListView::mouseMoveEvent(QMouseEvent* event)
{
    QListView::mouseMoveEvent(event);

    if (event != nullptr)
    {
        const QPoint      pos        = event->pos();
        const QModelIndex modelIndex = indexAt(pos);

        if (modelIndex.isValid())
        {
            setCursor(Qt::PointingHandCursor);
        }
        else
        {
            setCursor(Qt::ArrowCursor);
        }
    }
}

QSize NavigationListView::sizeHint() const
{
    QSize size_hint(kDefaultWidth_, fontMetrics().height());

    QAbstractListModel* model = qobject_cast<QAbstractListModel*>(this->model());
    if (model != nullptr && model->rowCount() > 0)
    {
        size_hint = QSize(GetListWidgetWidth() * kWidthPaddingFactor_, GetListWidgetHeight());
    }

    return size_hint;
}

int NavigationListView::GetListWidgetHeight() const
{
    int                 height = 0;
    QAbstractListModel* model  = qobject_cast<QAbstractListModel*>(this->model());
    if (model != nullptr)
    {
        int count = model->rowCount();
        for (int loop = 0; loop < count; loop++)
        {
            height += this->sizeHintForRow(loop);
        }
        height += this->frameWidth() * 2;
    }

    return height;
}

int NavigationListView::GetListWidgetWidth() const
{
    int                 width = 0;
    QAbstractListModel* model = qobject_cast<QAbstractListModel*>(this->model());
    if (model != nullptr)
    {
        for (int loop = 0; loop < model->rowCount(); loop++)
        {
            const int item_width = this->sizeHintForColumn(loop);
            if (width < item_width)
            {
                width = item_width;
            }
        }
    }
    return width;
}

void NavigationListView::OnScaleFactorChanged()
{
    // Invalidate font and fontmetrics by toggling the font size.
    QFont tmp_font = font();
    qreal original_point_size = tmp_font.pointSizeF();
    tmp_font.setPointSizeF(original_point_size + 1);
    setFont(tmp_font);
    tmp_font.setPointSizeF(original_point_size);
    setFont(tmp_font);

    updateGeometry();
}
