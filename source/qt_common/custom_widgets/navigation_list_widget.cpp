//=============================================================================
/// Copyright (c) 2018-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a navigation list widget
//=============================================================================

#include "navigation_list_widget.h"

#include <QDebug>
#include <QMouseEvent>
#include <QFont>

#include "common_definitions.h"
#include "scaling_manager.h"
#include "qt_util.h"

NavigationListWidget::NavigationListWidget(QWidget* parent)
    : QListWidget(parent)
{
    setMouseTracking(true);
    SetStyleSheet();

    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &NavigationListWidget::OnScaleFactorChanged);

    connect(&QtCommon::QtUtils::ColorTheme::Get(), &QtCommon::QtUtils::ColorTheme::ColorThemeUpdated, this, &NavigationListWidget::SetStyleSheet);
}

NavigationListWidget::~NavigationListWidget()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &NavigationListWidget::OnScaleFactorChanged);
}

void NavigationListWidget::mouseMoveEvent(QMouseEvent* event)
{
    QListWidget::mouseMoveEvent(event);

    if (event != nullptr)
    {
        const QPoint      position    = event->pos();
        const QModelIndex model_index = indexAt(position);

        if (model_index.isValid())
        {
            setCursor(Qt::PointingHandCursor);
        }
        else
        {
            setCursor(Qt::ArrowCursor);
        }
    }
}

QSize NavigationListWidget::sizeHint() const
{
    QSize size_hint(kDefaultWidth_, fontMetrics().height());

    if (count() > 0)
    {
        size_hint = QSize(GetListWidgetWidth(), GetListWidgetHeight());
    }

    return size_hint;
}

int NavigationListWidget::GetListWidgetHeight() const
{
    int count  = this->count();
    int height = 0;
    for (int loop = 0; loop < count; loop++)
    {
        height += this->sizeHintForRow(loop);
    }
    height += this->frameWidth() * 2;

    return height;
}

int NavigationListWidget::GetListWidgetWidth() const
{
    int width = 0;
    for (int loop = 0; loop < this->count(); loop++)
    {
        const int item_width = this->sizeHintForColumn(loop);
        if (width < item_width)
        {
            width = item_width;
        }
    }

    return width;
}

void NavigationListWidget::OnScaleFactorChanged()
{
    // Invalidate cached font size so that Qt's default painting can still be used.
    QFont tmp_font           = font();
    qreal original_pointsize = tmp_font.pointSizeF();
    tmp_font.setPointSizeF(original_pointsize + 1);
    setFont(tmp_font);
    tmp_font.setPointSizeF(original_pointsize);
    setFont(tmp_font);

    updateGeometry();
}

void NavigationListWidget::SetStyleSheet()
{
    if (QtCommon::QtUtils::ColorTheme::Get().GetColorTheme() == kColorThemeTypeLight)
    {
        setStyleSheet(kMainListWidgetStylesheet_);
    }
    else
    {
        setStyleSheet(kMainListWidgetDarkStylesheet_);
    }
}
