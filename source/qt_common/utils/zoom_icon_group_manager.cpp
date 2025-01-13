//=============================================================================
/// Copyright (c) 2019-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation for the zoom icon group manager class which handles
/// a group of zoom icons.
//=============================================================================

#include "zoom_icon_group_manager.h"

ZoomIconGroupManager::ZoomIconGroupManager(const ZoomIconManagerConfiguration& zoom_config)
{
    zoom_in_icon_ = new ZoomIconManager(zoom_config.zoom_in_button, zoom_config.zoom_in_resource_enabled, zoom_config.zoom_in_resource_disabled, true);
    zoom_out_icon_ = new ZoomIconManager(zoom_config.zoom_out_button, zoom_config.zoom_out_resource_enabled, zoom_config.zoom_out_resource_disabled, false);
    zoom_reset_icon_ = new ZoomIconManager(zoom_config.zoom_reset_button, zoom_config.zoom_reset_resource_enabled, zoom_config.zoom_reset_resource_disabled, false);
    zoom_to_selection_icon_ = new ZoomIconManager(zoom_config.zoom_to_selection_button, zoom_config.zoom_to_selection_resource_enabled, zoom_config.zoom_to_selection_resource_disabled, false);
}

ZoomIconGroupManager::~ZoomIconGroupManager()
{
    delete zoom_in_icon_;
    delete zoom_out_icon_;
    delete zoom_reset_icon_;
    delete zoom_to_selection_icon_;
}

void ZoomIconGroupManager::ZoomIn(bool zoom)
{
    // gray out the zoom out button if no further zoom possible
    zoom_in_icon_->SetEnabled(zoom);

    // zoomed in, so zoom out/zoom reset valid
    zoom_out_icon_->SetEnabled(true);
    zoom_reset_icon_->SetEnabled(true);
}

void ZoomIconGroupManager::ZoomOut(bool zoom)
{
    // gray out the zoom out button/reset zoom if no further zoom possible
    zoom_out_icon_->SetEnabled(zoom);
    zoom_reset_icon_->SetEnabled(zoom);

    // zoomed out, so zoom in is valid
    zoom_in_icon_->SetEnabled(true);
}

void ZoomIconGroupManager::ZoomToSelection(bool selected_region)
{
    zoom_to_selection_icon_->SetEnabled(selected_region);
}

void ZoomIconGroupManager::ZoomReset()
{
    zoom_in_icon_->SetEnabled(true);
    zoom_out_icon_->SetEnabled(false);
    zoom_reset_icon_->SetEnabled(false);
    zoom_to_selection_icon_->SetEnabled(false);
}

void ZoomIconGroupManager::SetButtonStates(bool zoom_in, bool zoom_out, bool zoom_selection, bool zoom_reset)
{
    zoom_in_icon_->SetEnabled(zoom_in);
    zoom_out_icon_->SetEnabled(zoom_out);
    zoom_reset_icon_->SetEnabled(zoom_reset);
    zoom_to_selection_icon_->SetEnabled(zoom_selection);
}

void ZoomIconGroupManager::GetButtonStates(bool& zoom_in, bool& zoom_out, bool& zoom_selection, bool& zoom_reset)
{
    zoom_in        = zoom_in_icon_->IsEnabled();
    zoom_out       = zoom_out_icon_->IsEnabled();
    zoom_reset     = zoom_reset_icon_->IsEnabled();
    zoom_selection = zoom_to_selection_icon_->IsEnabled();
}
