//=============================================================================
/// Copyright (c) 2019-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header for the zoom icon group manager class which handles a group
/// of zoom icons.
//=============================================================================

#ifndef QTCOMMON_UTILS_ZOOM_ICON_GROUP_MANAGER_H_
#define QTCOMMON_UTILS_ZOOM_ICON_GROUP_MANAGER_H_

#include <QPushButton>
#include "zoom_icon_manager.h"

/// @brief Configuration structure for a ZoomIconGroupManager.
typedef struct ZoomIconManagerConfiguration
{
    QPushButton* zoom_in_button;                       ///< The zoom in button.
    QString      zoom_in_resource_enabled;             ///< The resource for the zoom in icon when it is enabled.
    QString      zoom_in_resource_disabled;            ///< The resource for the zoom in icon when it is disabled.
    QPushButton* zoom_out_button;                      ///< The zoom out button.
    QString      zoom_out_resource_enabled;            ///< The resource for the zoom out icon when it is enabled.
    QString      zoom_out_resource_disabled;           ///< The resource for the zoom out icon when it is disabled.
    QPushButton* zoom_reset_button;                    ///< The zoom reset button.
    QString      zoom_reset_resource_enabled;          ///< The resource for the zoom reset icon when it is enabled.
    QString      zoom_reset_resource_disabled;         ///< The resource for the zoom reset icon when it is disabled.
    QPushButton* zoom_to_selection_button;             ///< The zoom to selection button.
    QString      zoom_to_selection_resource_enabled;   ///< The resource for the zoom to selection icon when it is enabled.
    QString      zoom_to_selection_resource_disabled;  ///< The resource for the zoom to selection icon when it is disabled.
} ZoomIconManagerConfiguration;

/// Manages the interactions between zoom in, zoom out, zoom to selection,
/// and zoom reset buttons.
class ZoomIconGroupManager
{
public:
    /// Constructor
    /// \param zoom_config The configuration for this object.
    explicit ZoomIconGroupManager(const ZoomIconManagerConfiguration& zoom_config);

    /// Destructor
    ~ZoomIconGroupManager();

    /// Set up the icons for the 'zoom in' button.
    /// \param zoom Is any further zoom in possible? True if so, false
    /// if fully zoomed in
    void ZoomIn(bool zoom);

    /// Set up the icons for the 'zoom out' button.
    /// \param zoom Is any further zoom out possible? True if so, false
    /// if fully zoomed out
    void ZoomOut(bool zoom);

    /// Set up the icons for the 'zoom to selection' button.
    /// \param selected_region Has a region been selected? True if so.
    void ZoomToSelection(bool selected_region);

    /// Set up the icons for the 'zoom reset' button.
    void ZoomReset();

private:
    ZoomIconManager* zoom_in_icon_;            ///< The zoom in icon.
    ZoomIconManager* zoom_out_icon_;           ///< The zoom out icon.
    ZoomIconManager* zoom_reset_icon_;         ///< The zoom reset icon.
    ZoomIconManager* zoom_to_selection_icon_;  ///< The zoom to selection icon.
};

#endif  // QTCOMMON_UTILS_ZOOM_ICON_GROUP_MANAGER_H_
