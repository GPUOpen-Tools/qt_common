//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of custom Tab Widget.  Adds support for right
/// justified tabs.
//=============================================================================

#include "TabWidget.h"

#include <QResizeEvent>

//-----------------------------------------------------------------------------
/// Explicit constructor
/// \param pParent The TabWidget's parent widget.
//-----------------------------------------------------------------------------
TabWidget::TabWidget(QWidget* pParent) :
    QTabWidget(pParent)
{
    m_tabBar.setParent(this);
    setTabBar(&m_tabBar);  // Replace the TabWidget's QTabBar with a custom one.
}

//-----------------------------------------------------------------------------
/// Returns the TabWidget's QTabBar object.
/// \return A pointer to the tabBar associated with this TabWidget.
//-----------------------------------------------------------------------------
QTabBar * TabWidget::tabBar() const
{
    return QTabWidget::tabBar();
}

//-----------------------------------------------------------------------------
/// Returns the QTabBar's height.
/// \return Height in pixels.
//-----------------------------------------------------------------------------
int TabWidget::TabHeight() const
{
    return tabBar()->sizeHint().height();
}

//-----------------------------------------------------------------------------
/// Handle a resize event.
/// \param pResizeEvent The resize event.
//-----------------------------------------------------------------------------
void TabWidget::resizeEvent(QResizeEvent* pResizeEvent)
{
    m_tabBar.resize(pResizeEvent->size());
    QTabWidget::resizeEvent(pResizeEvent);
}

//-----------------------------------------------------------------------------
/// Enable or disable tabs.
/// \param index The tab index to enable or disable
/// \param enable Set to true to enable or false to disable the tab.
//-----------------------------------------------------------------------------
void TabWidget::setTabEnabled(int index, bool enable)
{
    m_tabBar.setTabEnabled(index, enable);
}

//-----------------------------------------------------------------------------
/// Sets the index of the tab used as a spacer between left justified tabs and
/// right justified tabs.
/// \param index The index of the tab used as a spacer.
//-----------------------------------------------------------------------------
void TabWidget::SetSpacerIndex(const int index)
{
    m_tabBar.SetSpacerIndex(index);
}

//-----------------------------------------------------------------------------
/// Replaces a tab with a widget (e.g. a toolbar button)
/// \param index The index on the tab bar to be replaced.
/// \Param pWidget A pointer to a button widget that will replace the tab
//-----------------------------------------------------------------------------
void TabWidget::SetTabTool(int index, QWidget* pToolWidget)
{
    m_tabBar.SetTabTool(index, pToolWidget);
}
