//=============================================================================
/// Copyright (c) 2022-2023 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Header for a shared isa branch label navigation widget.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_BRANCH_LABEL_NAVIGATION_WIDGET_H_
#define QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_BRANCH_LABEL_NAVIGATION_WIDGET_H_

#include <QModelIndex>

#include "arrow_icon_combo_box.h"
#include "navigation_bar.h"
#include "shared_isa_widget.h"

/// @brief SharedIsaBranchLabelNavigationWidget extends the NavigationBar by adding a history combo box in between the arrow buttons.
class SharedIsaBranchLabelNavigationWidget final : public NavigationBar
{
    Q_OBJECT

public:
    /// @brief Constructor; create the history combo box, setup slots and change the buttons' style.
    ///
    /// @param [in] parent The parent widget.
    explicit SharedIsaBranchLabelNavigationWidget(QWidget* parent = nullptr);

    /// @brief Destructor.
    ~SharedIsaBranchLabelNavigationWidget();

    /// @brief Initialize the history combo box with an appropriate parent so it can render itself correctly.
    ///
    /// @param [in] combo_box_parent The widget to use to initialize the combo box.
    void InitializeHistoryComboBox(QWidget* combo_box_parent);

    /// @brief Clear all entries from history and reset the navigation buttons' status.
    void ClearHistory();

public slots:

    /// @brief Add a branch or label's destination source index as an new entry in the history.
    ///
    /// Do not add duplicate consecutive entries and trim any entries between the current index and the end of the history.
    /// Reset the selection and enable the back button.
    ///
    /// @param [in] The source model index of the destination branch or label.
    void AddBranchOrLabelToHistory(QModelIndex branch_label_source_index);

    /// @brief Navigate backwards one entry in history and disable the back button if necessary.
    void BackPressed();

    /// @brief Navigate forwards one entry in history and disable the forward button if necessary.
    void ForwardPressed();

    /// @brief Navigate to an entry in the history and update the back and fowards buttons if necessary.
    ///
    /// @param [in] item The item of the entry that was selected.
    void HistoryEntrySelected(QListWidgetItem* item);

signals:

    /// @brief Listeners can use this signal to know when to scroll to the given source model index.
    ///
    /// @param [in] branch_label_source_index The model index that was just selected using this navigation widget.
    void Navigate(QModelIndex branch_label_source_index);

private:
    /// @brief Trim any entries between the current index and the end of the history.
    void TrimHistory();

    /// @brief Sets the icons of the button widget.
    void SetButtonIcons();

    ArrowIconComboBox* branch_label_history_combo_;  ///< The combo box that holds the navigation history.
    int                history_index_;               ///< The current location into the navigation history.
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_BRANCH_LABEL_NAVIGATION_WIDGET_H_
