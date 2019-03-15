//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of custom navigation bar
//=============================================================================
#include <QMouseEvent>

#include "NavigationBar.h"

static const QString sNavigationBackButtonStyle = "IconButton {  margin: 0px; padding: 0px; border: none; background-color: rgb(51,51,51); background: rgb(51,51,51); color: #666;  min-width: 30px; min-height: 30px; max-width: 30px; max-height: 30px;} ";
static const QString gs_QT_STRING_BROWSE_BACK_NORMAL_RESOURCE = ":/Resources/assets/browse_back_normal.svg";
static const QString gs_QT_STRING_BROWSE_BACK_PRESSED_RESOURCE = ":/Resources/assets/browse_back_pressed.svg";
static const QString gs_QT_STRING_BROWSE_BACK_DISABLED_RESOURCE = ":/Resources/assets/browse_back_disabled.svg";
static const QString gs_QT_STRING_BROWSE_FWD_NORMAL_RESOURCE = ":/Resources/assets/browse_fwd_normal.svg";
static const QString gs_QT_STRING_BROWSE_FWD_PRESSED_RESOURCE = ":/Resources/assets/browse_fwd_pressed.svg";
static const QString gs_QT_STRING_BROWSE_FWD_DISABLED_RESOURCE = ":/Resources/assets/browse_fwd_disabled.svg";

//-----------------------------------------------------------------------------
/// Explicit constructor
/// \param pParent The NavigationBar's parent widget.
//-----------------------------------------------------------------------------
NavigationBar::NavigationBar(QWidget* pParent) :
    QAbstractButton(pParent),
    m_layout(this),
    m_browseBackButton(this, gs_QT_STRING_BROWSE_BACK_NORMAL_RESOURCE, gs_QT_STRING_BROWSE_BACK_NORMAL_RESOURCE, gs_QT_STRING_BROWSE_BACK_PRESSED_RESOURCE, gs_QT_STRING_BROWSE_BACK_DISABLED_RESOURCE),
    m_browseForwardButton(this, gs_QT_STRING_BROWSE_FWD_NORMAL_RESOURCE, gs_QT_STRING_BROWSE_FWD_NORMAL_RESOURCE, gs_QT_STRING_BROWSE_FWD_PRESSED_RESOURCE, gs_QT_STRING_BROWSE_FWD_DISABLED_RESOURCE)
{
    m_layout.setAlignment(Qt::AlignCenter);
    m_layout.setSpacing(10);
    m_layout.setMargin(20);

    m_browseBackButton.setStyleSheet(sNavigationBackButtonStyle);
    EnableBackButton(false);

    m_browseForwardButton.setStyleSheet(sNavigationBackButtonStyle);
    EnableForwardButton(false);

    m_layout.insertWidget(0, &m_browseBackButton);
    m_layout.insertWidget(1, &m_browseForwardButton);
    setMouseTracking(true);
}

//-----------------------------------------------------------------------------
/// Handle mouse move event.  Change cursor back to an arrow.
/// \param pEvent The mouseMoveEvent event.
//-----------------------------------------------------------------------------
void NavigationBar::mouseMoveEvent(QMouseEvent* pEvent)
{
    Q_UNUSED(pEvent);
    setCursor(Qt::ArrowCursor);
}

//-----------------------------------------------------------------------------
/// Enable or disable the back button.  Set the mouse cursor to pointing hand
/// or an arrow.
/// \param enable Set to true to enale the button or false to disable it.
//-----------------------------------------------------------------------------
void NavigationBar::EnableBackButton(bool enable)
{
    m_browseBackButton.setEnabled(enable);
    if (enable)
    {
        m_browseBackButton.setCursor(Qt::PointingHandCursor);
    }
    else
    {
        m_browseBackButton.setCursor(Qt::ArrowCursor);
    }
}

//-----------------------------------------------------------------------------
/// Enable or disable the forward button.  Set the mouse cursor to pointing hand
/// or an arrow.
/// \param enable Set to true to enale the button or false to disable it.
//-----------------------------------------------------------------------------
void NavigationBar::EnableForwardButton(bool enable)
{
    m_browseForwardButton.setEnabled(enable);
    if (enable)
    {
        m_browseForwardButton.setCursor(Qt::PointingHandCursor);
    }
    else
    {
        m_browseForwardButton.setCursor(Qt::ArrowCursor);
    }
}
