//=============================================================================
/// Copyright (c) 2019-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header for the zoom icon manager class. Handles the zoom icon
/// state and modifies the icon to show the correct state
//=============================================================================

#ifndef QTCOMMON_UTILS_ZOOM_ICON_MANAGER_H_
#define QTCOMMON_UTILS_ZOOM_ICON_MANAGER_H_

#include <QString>

class QPushButton;

class ZoomIconManager
{
public:
    /// Constructor
    /// \param icon The button for this icon.
    /// \param enabled_asset The resource asset for when this icon is enabled.
    /// \param disabled_asset The resource asset for when this icon is disabled.
    /// \param enabled Whether or not the icon should default to enabled or disabled.
    explicit ZoomIconManager(QPushButton* icon, QString enabled_asset, QString disabled_asset, bool enabled);

    /// Destructor
    ~ZoomIconManager();

    /// Set the state of a zoom icon and setup the correct icon resource
    /// depending on that state
    /// \param enable Is the state to be enabled
    void SetEnabled(bool enable);

    /// @brief Get the state of a zoom icon.
    ///
    /// @return true if the zoom icon is enabled, false otherwise.
    bool IsEnabled();

private:
    QPushButton* icon_;           ///< The ui icon widget
    QString      enabled_asset_;  ///< The asset corresponding to the enabled state
    QString      disabled_asset_; ///< The asset corresponding to the disabled state
};

#endif  // QTCOMMON_UTILS_ZOOM_ICON_MANAGER_H_
