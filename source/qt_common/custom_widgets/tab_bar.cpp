//=============================================================================
/// Copyright (c) 2017-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of custom Tab Bar.
//=============================================================================
#include "tab_bar.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QSizePolicy>
#include <QTabBar>
#include <QWidget>

#include "qt_util.h"
#include "scaling_manager.h"

TabBar::TabBar(QWidget* parent)
    : QTabBar(parent)
    , stretch_last_(false)
    , spacer_index_(-1)
    , mouse_hover_last_tab_index_(-1)
{
    setMouseTracking(true);

    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &TabBar::OnScaleFactorChanged);
}

TabBar::~TabBar()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &TabBar::OnScaleFactorChanged);
}

void TabBar::mouseMoveEvent(QMouseEvent* event)
{
    int tab_index = QTabBar::tabAt(event->pos());

    // Only change mouse cursor if the mouse is hovoring over a different tab.
    if (mouse_hover_last_tab_index_ != tab_index)
    {
        if (isTabEnabled(tab_index))
        {
            setCursor(Qt::PointingHandCursor);
        }
        else
        {
            setCursor(Qt::ArrowCursor);
        }
        mouse_hover_last_tab_index_ = tab_index;
    }
}

void TabBar::OnScaleFactorChanged()
{
    QtCommon::QtUtils::InvalidateFontMetrics(this);

    updateGeometry();
}

void TabBar::setTabEnabled(int index, bool enable)
{
    // Force next mouseMoveEvent to set mouse cursor even if mouse is hovoring over
    // the same tab as the last event call.
    mouse_hover_last_tab_index_ = -1;

    QTabBar::setTabEnabled(index, enable);
}

QSize TabBar::minimumTabSizeHint(int index) const
{
    if (index == SpacerIndex())
    {
        return QSize(0, QTabBar::tabSizeHint(index).height());
    }
    else
    {
        return QTabBar::minimumTabSizeHint(index);
    }
}

QSize TabBar::tabSizeHint(int index) const
{
    // Make sure to polish the tab bar. This makes
    // sure it has the actual font from the stylesheet
    this->ensurePolished();

    int height = QTabBar::tabSizeHint(index).height();
    if (index == SpacerIndex())
    {
        return QSize(CalcSpacerWidth(), height);
    }
    else if (tabText(index).isEmpty())
    {
        int      width  = 0;
        QWidget* widget = tabButton(index, QTabBar::ButtonPosition::LeftSide);
        if (widget)
        {
            width = widget->geometry().width();
        }

        widget = tabButton(index, QTabBar::ButtonPosition::RightSide);
        if (widget)
        {
            width = widget->geometry().width();
        }
        return QSize(width, height);
    }
    else
    {
        QSize size_hint = QTabBar::tabSizeHint(index);
        size_hint.setWidth(size_hint.width() + contentsMargins().left() + contentsMargins().right());
        size_hint.setHeight(size_hint.height() + contentsMargins().top() + contentsMargins().bottom());
        return size_hint;
    }
}

void TabBar::SetSpacerIndex(int index)
{
    if (index != -1)
    {
        setTabEnabled(index, false);
        setTabText(index, "");
        adjustSize();
    }
    spacer_index_ = index;
}

void TabBar::SetStretchLast(bool stretch)
{
    stretch_last_ = stretch;
}

void TabBar::SetTabTool(int index, QWidget* button_widget, QTabBar::ButtonPosition pos)
{
    setTabText(index, "");
    setTabEnabled(index, false);
    setTabButton(index, pos, button_widget);
}

int TabBar::SpacerIndex() const
{
    return spacer_index_;
}

int TabBar::CalcSpacerWidth() const
{
    if ((count() == 0) || (spacer_index_ < 0))
    {
        return 0;
    }

    int spacer_width = parentWidget()->width();
    for (int i = 0; i < count(); i++)
    {
        if (i != spacer_index_)
        {
            spacer_width -= tabRect(i).width();
        }
    }

    return spacer_width;
}
