//=============================================================================
// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief  Implementation of the custom driver overrides notification banner.
//=============================================================================

#include "driver_overrides_notification_banner.h"
#include "ui_banner_widget.h"

#include "qt_util.h"
#include "scaled_label.h"

#include "driver_overrides_model.h"

DriverOverridesNotificationBanner::DriverOverridesNotificationBanner(QWidget* parent)
    : BannerWidget(parent)
{
    Init();
}

DriverOverridesNotificationBanner::~DriverOverridesNotificationBanner()
{
}

void DriverOverridesNotificationBanner::Init()
{
    // Map the dynamic model properties to the UI elements.
    driver_overrides::DriverOverridesModel::GetInstance()->BindWidgetToModelAttribute(
        driver_overrides::kModelAttributeShowNotification, this, "visible", &banner_message_mapper_);
    driver_overrides::DriverOverridesModel::GetInstance()->BindWidgetToModelAttribute(
        driver_overrides::kModelAttributeNameNotificationMessage, ui_->message_label_, "text", &banner_message_mapper_);
    driver_overrides::DriverOverridesModel::GetInstance()->BindWidgetToModelAttribute(
        driver_overrides::kModelAttributeEnableSeeDetailsLink, ui_->show_details_label_, "enabled", &see_details_link_mapper_);
}

void DriverOverridesNotificationBanner::DontShowAgainQuery()
{
    // Get the title and message text from the model.
    driver_overrides::DriverOverridesModel* driver_overrides_model = driver_overrides::DriverOverridesModel::GetInstance();
    const QString& title_text   = driver_overrides_model->GetModelAttributeValue(driver_overrides::kModelAttributeNameDontShowMessageBoxTitle).toString();
    const QString& message_text = driver_overrides_model->GetModelAttributeValue(driver_overrides::kModelAttributeNameDontShowMessageBoxText).toString();

    // Show a message box to confirm the user wants to disable notifications.
    const int response = QtCommon::QtUtils::ShowMessageBox(this, QMessageBox::Yes | QMessageBox::No, QMessageBox::Warning, title_text, message_text);

    if (response == QMessageBox::Yes)
    {
        // Update the model to disable notifications.
        driver_overrides_model->SetModelAttributeValue(driver_overrides::kModelAttributeNameEnableNotifications, false);
        driver_overrides_model->SetModelAttributeValue(driver_overrides::kModelAttributeShowNotification, false);

        // Emit the signal to notify the application that the persistant setting needs to be updated.
        emit DontShowAgainRequested();
    }
}

QColor DriverOverridesNotificationBanner::GetLinkColor() const
{
    return link_color_;
}

QColor DriverOverridesNotificationBanner::GetDisabledLinkColor() const
{
    return disabled_link_color_;
}

void DriverOverridesNotificationBanner::SetLinkColor(const QColor color)
{
    link_color_ = color;
    BannerWidget::SetLinkColor(color);
}

void DriverOverridesNotificationBanner::SetDisabledLinkColor(const QColor color)
{
    disabled_link_color_ = color;
    BannerWidget::SetDisabledLinkColor(color);
}
