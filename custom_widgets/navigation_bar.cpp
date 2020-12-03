//=============================================================================
/// Copyright (c) 2017-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of custom navigation bar
//=============================================================================
#include "navigation_bar.h"

#include <QMouseEvent>

#include "utils/scaling_manager.h"

NavigationBar::NavigationBar(QWidget* parent)
    : QAbstractButton(parent)
    , layout_(this)
    , browse_back_button_(this, kBrowseBackNormalResource_, kBrowseBackNormalResource_, kBrowseBackPressedResource_, kBrowseBackDisabledResource_)
    , browse_forward_button_(this,
                             kBrowseForwardNormalResource_,
                             kBrowseForwardNormalResource_,
                             kBrowseForwardPressedResource_,
                             kBrowseForwardDisabledresource_)
{
    layout_.setAlignment(Qt::AlignCenter);
    layout_.setSpacing(10);
    layout_.setMargin(20);

    browse_back_button_.setStyleSheet(kNavigationButtonStyle_);
    EnableBackButton(false);

    browse_forward_button_.setStyleSheet(kNavigationButtonStyle_);
    EnableForwardButton(false);

    // Set both buttons to have a base size of 30x30 pixels.
    browse_back_button_.setBaseSize(QSize(30, 30));
    browse_forward_button_.setBaseSize(QSize(30, 30));

    layout_.insertWidget(0, &browse_back_button_);
    layout_.insertWidget(1, &browse_forward_button_);
    setMouseTracking(true);
}

void NavigationBar::mouseMoveEvent(QMouseEvent* pEvent)
{
    Q_UNUSED(pEvent);
    setCursor(Qt::ArrowCursor);
}

void NavigationBar::EnableBackButton(bool enable)
{
    browse_back_button_.setEnabled(enable);
    if (enable)
    {
        browse_back_button_.setCursor(Qt::PointingHandCursor);
    }
    else
    {
        browse_back_button_.setCursor(Qt::ArrowCursor);
    }
}

void NavigationBar::EnableForwardButton(bool enable)
{
    browse_forward_button_.setEnabled(enable);
    if (enable)
    {
        browse_forward_button_.setCursor(Qt::PointingHandCursor);
    }
    else
    {
        browse_forward_button_.setCursor(Qt::ArrowCursor);
    }
}
