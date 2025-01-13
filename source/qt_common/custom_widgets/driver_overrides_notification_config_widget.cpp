//=============================================================================
// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief  Implementation of the custom Driver Overrides notification configuration widget.
//=============================================================================

#include "driver_overrides_notification_config_widget.h"
#include "ui_driver_overrides_notification_config_widget.h"

#include "driver_overrides_model.h"
#include "scaled_label.h"

#include <QHBoxLayout>

DriverOverridesNotificationConfigWidget::DriverOverridesNotificationConfigWidget(QWidget* parent)
    : QWidget(parent)
    , ui_(new Ui::DriverOverridesNotificationConfigWidget)
{
    ui_->setupUi(this);
}

DriverOverridesNotificationConfigWidget::~DriverOverridesNotificationConfigWidget()
{
}

bool DriverOverridesNotificationConfigWidget::Init(const bool check_box_state, const bool include_on_off_indicator)
{
    bool     result                 = true;
    auto     driver_overrides_model = driver_overrides::DriverOverridesModel::GetInstance();
    QWidget* check_box_label_widget = nullptr;  // The widget that will be used as the label for the check box.
    QString  check_box_label_property;          // The property of the widget that will be used to update the label text.

    if (include_on_off_indicator)
    {
        // Set the widget and property to use for the check box label.
        check_box_label_widget   = ui_->driver_overrides_notification_check_box_label_;
        check_box_label_property = "text";
    }
    else
    {
        // Delete unnecessary widgets.

        // Remove left_horizontal_spacer_ from the horizontal layout.
        int spacer_id = ui_->horizontal_layout_->indexOf(ui_->left_horizontal_spacer_);
        Q_ASSERT(spacer_id != -1);
        if (spacer_id != -1)
        {
            ui_->horizontal_layout_->removeItem(ui_->horizontal_layout_->itemAt(spacer_id));
        }

        // Remove the label (in this mode, the check box's label is used instead).
        ui_->driver_overrides_notification_check_box_label_->deleteLater();

        // Set the widget and property to use for the check box label.
        check_box_label_widget   = ui_->driver_overrides_notification_check_box_;
        check_box_label_property = "label_text";
    }

    // Map the model data to the widgets.
    result &= driver_overrides_model->BindWidgetToModelAttribute(driver_overrides::kModelAttributeNameNotificationSettingLabel,
                                                                 check_box_label_widget,
                                                                 check_box_label_property,
                                                                 &driver_overrides_checkbox_text_mapper_);
    Q_ASSERT(result);

    result &= driver_overrides_model->BindWidgetToModelAttribute(driver_overrides::kModelAttributeNameEnableNotifications,
                                                                 ui_->driver_overrides_notification_check_box_,
                                                                 "checked",
                                                                 &driver_overrides_checkbox_state_mapper_);
    Q_ASSERT(result);

    result &= driver_overrides_model->BindWidgetToModelAttribute(driver_overrides::kModelAttributeNameNotificationSettingTitle,
                                                                 ui_->driver_overrides_settings_title_label_,
                                                                 "text",
                                                                 &driver_overrides_widget_text_mapper_);
    Q_ASSERT(result);

    // Update the model with the current state of the check box.
    result &= driver_overrides_model->SetModelAttributeValue(driver_overrides::kModelAttributeNameEnableNotifications, check_box_state);
    Q_ASSERT(result);

    // Set up a connection to handle when the check box is clicked.
    connect(ui_->driver_overrides_notification_check_box_, &QCheckBox::clicked, this, &DriverOverridesNotificationConfigWidget::HandleCheckBoxClicked);

    return result;
}

void DriverOverridesNotificationConfigWidget::HandleCheckBoxClicked(bool checked)
{
    emit StateChanged(checked);
}
