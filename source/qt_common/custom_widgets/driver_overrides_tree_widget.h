//=============================================================================
// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Header file for a widget that displays the driver overrides tree.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_DRIVER_OVERRIDES_TREE_WIDGET_H_
#define QTCOMMON_CUSTOM_WIDGETS_DRIVER_OVERRIDES_TREE_WIDGET_H_

#include <QDataWidgetMapper>
#include <QVBoxLayout>
#include <QWidget>

namespace Ui
{
    class DriverOverridesTreeWidget;
}

/// @brief A widget that displays the driver overrides tree.
class DriverOverridesTreeWidget : public QWidget
{
    Q_OBJECT
public:
    /// @brief Constructor
    /// @param [in] parent                          The parent of the DriverOverridesTreeWidget
    explicit DriverOverridesTreeWidget(QWidget* parent = nullptr);

    /// @brief Destructor.
    virtual ~DriverOverridesTreeWidget();

    /// @brief Create and configure the sub-widgets and layouts for the DriverOverridesTreeWidget.
    void Init();

    /// @brief Event filter to handle displaying the treeview tooltips.
    /// @param [in] object                          The object that the event is for.
    /// @param [in] event                           The event that is being processed.
    /// @return True if the event was handled, false otherwise.
    bool eventFilter(QObject* object, QEvent* event);

private slots:
    /// @brief Slot to handle when the model has been updated.
    void UpdateView();

private:
    Ui::DriverOverridesTreeWidget* ui_;  ///< The Qt ui form.

    QDataWidgetMapper label_title_text_mapper_;  ///< The data widget mapper for the title label text.
    QDataWidgetMapper tree_view_mapper_;         ///< The data widget mapper for the tree view.
};
#endif  // QTCOMMON_CUSTOM_WIDGETS_DRIVER_OVERRIDES_TREE_WIDGET_H_
