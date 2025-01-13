//=============================================================================
// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief  Header for the custom driver overrides notification banner.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_DRIVER_OVERRIDES_NOTIFICATION_BANNER_H_
#define QTCOMMON_CUSTOM_WIDGETS_DRIVER_OVERRIDES_NOTIFICATION_BANNER_H_

#include "banner_widget.h"

#include <QDataWidgetMapper>

/// @brief Custom widget for the driver overrides notification banner.
class DriverOverridesNotificationBanner : public BannerWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor linkColor READ GetLinkColor WRITE SetLinkColor)
    Q_PROPERTY(QColor disabledLinkColor READ GetDisabledLinkColor WRITE SetDisabledLinkColor)

public:
    /// @brief Constructor
    ///
    /// @param [in] parent                          The parent of the banner widget.
    explicit DriverOverridesNotificationBanner(QWidget* parent);

    /// @brief Destructor.
    virtual ~DriverOverridesNotificationBanner();

    /// @brief Show a message box to confirm the user wants to disable notifications.
    virtual void DontShowAgainQuery();

    /// @brief Get the link text color for the labels.
    ///
    /// @return The link text color.
    QColor GetLinkColor() const;

    /// @brief Get the disabled link text color for the labels.
    ///
    /// @return The disabled link text color.
    QColor GetDisabledLinkColor() const;

    /// @brief Set the link text color for the labels.
    ///
    /// @param [in] color                           The color to set.
    void SetLinkColor(const QColor color);

    /// @brief Set the disabled link text color for the labels.
    ///
    /// @param [in] color                           The color to set.
    void SetDisabledLinkColor(const QColor color);

private:
    /// @brief Intializes the widget.
    void Init();

private:
    QDataWidgetMapper banner_message_mapper_;    ///< Data widget mapper for binding model properties to banner UI elements.
    QDataWidgetMapper see_details_link_mapper_;  ///< Data widget mapper for binding model enable property to the "see details" link.

    QColor link_color_;           ///< The text color for links.
    QColor disabled_link_color_;  ///< The text color for disabled links.
};
#endif  // QTCOMMON_CUSTOM_WIDGETS_DRIVER_OVERRIDES_NOTIFICATION_BANNER_H_
