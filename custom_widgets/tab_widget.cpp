//=============================================================================
/// Copyright (c) 2017-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of custom Tab Widget which adds support for right
/// justified tabs.
//=============================================================================

#include "tab_widget.h"
#include "utils/scaling_manager.h"

#include <QResizeEvent>

TabWidget::TabWidget(QWidget* parent)
    : QTabWidget(parent)
{
    tab_bar_.setParent(this);

    // Replace the TabWidget's QTabBar with a custom one.
    setTabBar(&tab_bar_);

    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &QTabWidget::updateGeometry);
}

TabWidget::~TabWidget()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &QTabWidget::updateGeometry);
}

QTabBar* TabWidget::tabBar() const
{
    return QTabWidget::tabBar();
}

int TabWidget::TabHeight() const
{
    return tabBar()->height();
}

void TabWidget::resizeEvent(QResizeEvent* resize_event)
{
    tab_bar_.resize(resize_event->size());
    QTabWidget::resizeEvent(resize_event);
}

void TabWidget::setTabEnabled(int index, bool enable)
{
    tab_bar_.setTabEnabled(index, enable);
}

void TabWidget::SetSpacerIndex(const int index)
{
    tab_bar_.SetSpacerIndex(index);
}

void TabWidget::SetStretchLast(const bool stretch)
{
    tab_bar_.SetStretchLast(stretch);
}

void TabWidget::SetTabTool(int index, QWidget* tool_widget, QTabBar::ButtonPosition pos)
{
    tab_bar_.SetTabTool(index, tool_widget, pos);
}
