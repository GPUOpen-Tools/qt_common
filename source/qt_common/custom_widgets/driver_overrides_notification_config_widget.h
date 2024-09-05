//=============================================================================
// Copyright (c) 2024 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief  Header for the custom Driver Overrides notification configuration widget.
///
/// This DriverOverridesNotificationConfigWidget is a container for a checkbox
/// and a label that is used to enable or disable showing the Driver Overrides
/// notification banner.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_DRIVER_OVERRIDES_NOTIFICATION_CONFIG_WIDGET_H_
#define QTCOMMON_CUSTOM_WIDGETS_DRIVER_OVERRIDES_NOTIFICATION_CONFIG_WIDGET_H_

#include <QDataWidgetMapper>
#include <QWidget>

namespace Ui
{
    class DriverOverridesNotificationConfigWidget;
}

/// @brief A custom widget with a checkbox setting for the Driver Overrides Notification.
class DriverOverridesNotificationConfigWidget : public QWidget
{
    Q_OBJECT
public:
    /// @brief Constructor
    ///
    /// @param [in] parent                          The notification settings widget.
    explicit DriverOverridesNotificationConfigWidget(QWidget* parent = nullptr);

    /// @brief Destructor.
    virtual ~DriverOverridesNotificationConfigWidget();

    /// @brief Map the Driver Overrides model to the checkbox and label widgets.
    ///
    /// @param [in] check_box_state                 The initial state of the checkbox loaded from persistant storage.
    /// @param [in] include_on_off_indicator        Flag that, if true, causes the on/off indicator to be shown.
    ///
    /// @return True if the widget was initialized successfully, false otherwise.
    bool Init(const bool check_box_state, const bool include_on_off_indicator);

signals:
    /// @brief Signal emitted the checkbox state has changed.
    ///
    /// @param [in] checked                            The state of the check box.
    void StateChanged(bool checked = false);

private slots:
    /// @brief Slot to handle when the checkBox is clicked.
    ///
    /// @param [in] checked                            The state of the check box.
    void HandleCheckBoxClicked(bool checked = false);

protected:
    Ui::DriverOverridesNotificationConfigWidget* ui_;  ///< The UI object for the widget.

    QDataWidgetMapper driver_overrides_widget_text_mapper_;  ///< Data widget mapper for binding model properties Driver Overrides don't show again label.
    QDataWidgetMapper
        driver_overrides_checkbox_text_mapper_;  ///< Data widget mapper for binding model properties to Driver Overrides don't show again checkbox.
    QDataWidgetMapper
        driver_overrides_checkbox_state_mapper_;  ///< Data widget mapper for binding model properties to Driver Overrides don't show again checkbox.
};
#endif  // QTCOMMON_CUSTOM_WIDGETS_DRIVER_OVERRIDES_NOTIFICATION_CONFIG_WIDGET_H_
