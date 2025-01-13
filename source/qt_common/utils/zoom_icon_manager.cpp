//=============================================================================
/// Copyright (c) 2019-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation for the zoom icon manager class.
//=============================================================================

#include <QPushButton>

#include "zoom_icon_manager.h"

ZoomIconManager::ZoomIconManager(QPushButton* icon, QString enabled_asset, QString disabled_asset, bool enabled)
    : icon_(icon)
    , enabled_asset_(enabled_asset)
    , disabled_asset_(disabled_asset)
{
    icon_->setCursor(Qt::PointingHandCursor);
    SetEnabled(enabled);
}

ZoomIconManager::~ZoomIconManager()
{
}

void ZoomIconManager::SetEnabled(bool enable)
{
    if (enable == true)
    {
        icon_->setIcon(QIcon(enabled_asset_));
        icon_->setEnabled(true);
    }
    else
    {
        icon_->setIcon(QIcon(disabled_asset_));
        icon_->setEnabled(false);
    }
}

bool ZoomIconManager::IsEnabled()
{
    return icon_->isEnabled();
}
