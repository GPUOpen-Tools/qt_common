//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of custom Tab Bar
//=============================================================================
#include <QTabBar>
#include <QMouseEvent>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QPainter>

#include "TabBar.h"
#include <../Scaling/ScalingManager.h>

//-----------------------------------------------------------------------------
/// Explicit constructor
/// \param pParent The TabBar's parent widget.
//-----------------------------------------------------------------------------
TabBar::TabBar(QWidget* pParent) :
    QTabBar(pParent),
    m_spacerIndex(-1),
    m_mouseHoverLastTabIndex(-1)
{
    setMouseTracking(true);
}

//-----------------------------------------------------------------------------
/// Handle mouse move event.  Change cursor depending on state of tab
/// that the mouse is hovering over.
/// \param pEvent The mouseMoveEvent event.
//-----------------------------------------------------------------------------
void TabBar::mouseMoveEvent(QMouseEvent* pEvent)
{
    int tabIndex = QTabBar::tabAt(pEvent->pos());

    // Only change mouse cursor if the mouse is hovoring over a different tab.
    if (m_mouseHoverLastTabIndex != tabIndex)
    {
        if (isTabEnabled(tabIndex))
        {
            setCursor(Qt::PointingHandCursor);
        }
        else
        {
            setCursor(Qt::ArrowCursor);
        }
        m_mouseHoverLastTabIndex = tabIndex;
    }
}

//-----------------------------------------------------------------------------
/// Enable or disable tabs.  Also resets the tab that the mouse was last detected
/// to be hovering over.
/// \param index The tab index to enable or disable
/// \param enable Set to true to enable or false to disable the tab.
//-----------------------------------------------------------------------------
void TabBar::setTabEnabled(int index, bool enable)
{
    // Force next mouseMoveEvent to set mouse cursor even if mouse is hovoring over
    // the same tab as the last event call.
    m_mouseHoverLastTabIndex = -1;

    QTabBar::setTabEnabled(index, enable);
}

//-----------------------------------------------------------------------------
/// Override for returning the minimum tab size.  If the size of the spacer tab
/// is requested, return zero.  This allows the tab widget to shrink if it is
/// resized.
/// \param index The tab to retrieve the size.
/// \return A QSize object containing the width and height of the tab.
//-----------------------------------------------------------------------------
QSize TabBar::minimumTabSizeHint(int index) const
{
    if (index == SpacerIndex() || tabText(index).isEmpty())
    {
        return QSize(0, QTabBar::tabSizeHint(index).height());
    }
    else
    {
        return QTabBar::minimumTabSizeHint(index);
    }
}

//-----------------------------------------------------------------------------
/// Override for returning the tab size.  If the size of the spacer tab
/// is requested, the width is calculated to fill in the space between the left
/// justified tabs and right justified tabs.
/// \param index The tab to retrieve the size.
/// \return A QSize object containing the width and height of the tab.
//-----------------------------------------------------------------------------
QSize TabBar::tabSizeHint(int index) const
{
    int height = QTabBar::tabSizeHint(index).height();
    if (index == SpacerIndex())
    {
        return QSize(CalcSpacerWidth(), height);
    }
    else if (tabText(index).isEmpty())
    {
        int width = 0;
        QWidget* pWidget = tabButton(index, QTabBar::ButtonPosition::LeftSide);
        if (pWidget)
        {
            width += pWidget->width();
        }

        pWidget = tabButton(index, QTabBar::ButtonPosition::RightSide);
        if (pWidget)
        {
            width += pWidget->width();
        }
        return QSize(width, height);
    }
    else
    {
        return QSize(QTabBar::tabSizeHint(index).width(), height);
    }
}

//-----------------------------------------------------------------------------
/// Overriden paint event for the TabBar class.
/// \param pEvent Pointer to the QPaintEvent object.
//-----------------------------------------------------------------------------
void TabBar::paintEvent(QPaintEvent* pEvent)
{
    QTabBar::paintEvent(pEvent);
    if (count() > 0)
    {
        QPainter painter;
        painter.begin(this);
        painter.setPen(tabTextColor(0));
        painter.drawLine(pEvent->rect().topLeft(), pEvent->rect().topRight());
        painter.end();
    }
}

//-----------------------------------------------------------------------------
/// Set the index of the tab which is used as a spacer between the left and
/// right justified tabs.
/// \param index The tab to be used as a spacer.
//-----------------------------------------------------------------------------
void TabBar::SetSpacerIndex(int index)
{
    if (index != -1)
    {
        setTabEnabled(index, false);
        setTabText(index, "");
    }
    m_spacerIndex = index;
}

//-----------------------------------------------------------------------------
/// Replaces a tab with a widget (e.g. a toolbar button)
/// \param index The index on the tab bar to be replaced.
/// \Param pWidget A pointer to a button widget that will replace the tab
/// (i.e. all tabs on the tab bar are left justified).
//-----------------------------------------------------------------------------
void TabBar::SetTabTool(int index, QWidget* pWidget)
{
    setTabText(index, "");
    setTabEnabled(index, false);
    setTabButton(index, QTabBar::LeftSide, pWidget);
}

//-----------------------------------------------------------------------------
/// Returns the index of the tab to be used as a spacer between the left justified
/// and right justified tabs.
/// \return The index of the spacer tab.  -1 indicates no spacer tab has been defined
/// (i.e. all tabs on the tab bar are left justified).
//-----------------------------------------------------------------------------
int TabBar::SpacerIndex() const
{
    return m_spacerIndex;
}

//-----------------------------------------------------------------------------
/// Calculates the width of the spacer by subtracting the width of the left and right
/// justified tabs from the total width of the tab bar.
/// \return The width of the spacer tab.
//-----------------------------------------------------------------------------
int TabBar::CalcSpacerWidth() const
{
    if ( (count() == 0) || (m_spacerIndex < 0) )
    {
        return 0;
    }

    QRect leftTabRect;
    int tabMargin = contentsMargins().left();
    if (count() > 1)
    {
        leftTabRect = tabRect(m_spacerIndex - 1);
        tabMargin = ScalingManager::Get().Scaled(4);
    }

    int rightTabsWidth = leftTabRect.width() * (count() - (m_spacerIndex + 1));
    int spacerWidth = parentWidget()->width() - (leftTabRect.right() + rightTabsWidth);
    spacerWidth -= (tabMargin * 2);
    return spacerWidth;
}
