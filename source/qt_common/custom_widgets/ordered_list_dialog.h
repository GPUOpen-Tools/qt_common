//=============================================================================
/// Copyright (c) 2025 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief  Header for our ordered list dialog
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_ORDERED_LIST_DIALOG_H_
#define QTCOMMON_CUSTOM_WIDGETS_ORDERED_LIST_DIALOG_H_

#include <memory>

#include <QDialog>
#include <QListWidgetItem>
#include <QString>
#include <QVBoxLayout>

namespace Ui
{
    class OrderedListViewDialog;
}

/// @brief Class that implements the ordered list dialog
class OrderedListDialog : public QDialog
{
    Q_OBJECT

public:
    /// @brief Constructor for OrderedListDialog.
    /// @param [in] delimiter The delimiter used to split the ordered list managed by this class.
    /// @param [in] window_icon The window icon for this dialog.
    /// @param [in] parent The parent widget.
    OrderedListDialog(const QString& delimiter, const QIcon& window_icon = {}, QWidget* parent = nullptr);

    /// @brief Destructor; Explicitly define an empty destructor, to be able to use unique_ptr for Ui::OrderedListViewDialog member function.
    virtual ~OrderedListDialog();

    /// @brief Initialize the list of entries.
    /// @param [in] entries The input entries for the dialog.
    void SetListItems(const QString& entries);

private slots:
    /// @brief Exit the dialog.
    void HandleExit(bool /* checked */);

    /// @brief Handler to delete the selected item.
    void HandleDeleteButtonClick(bool /* checked */);

    /// @brief Handler to edit the selected entry.
    void HandleEditButtonClick(bool /* checked */);

    /// @brief Handler when an entry changes.
    /// @param [in] item The list widget item that changed.
    void HandleListItemChanged(QListWidgetItem* item);

    /// @brief Handler when an entry is selected, this supports both clicking, and also using the arrow keys to change selection.
    void HandleListItemSelectionChanged();

    /// @brief Handler to move the selected item down one row.
    void HandleMoveDownButtonClick(bool /* checked */);

    /// @brief Handler to move the selected item up one row.
    void HandleMoveUpButtonClick(bool /* checked */);

    /// @brief Handler when the OK button is clicked.
    void HandleOKButtonClick(bool /* checked */);

    /// @brief Handler when the New button is clicked.
    void HandleNewButtonClick(bool /* checked */);

signals:
    /// @brief Signal emitted when the user clicks the "OK" button to close the dialog.
    void OKButtonClicked(QStringList);

protected:
    /// @brief Force clients to provide a way to handle edited list items.
    /// @param [in] item The list widget item that changed.
    virtual void OnListItemChanged(QListWidgetItem* item) = 0;

    /// @brief Insert a blank item in the list widget.
    void InsertBlankItem();

    /// @brief Update the buttons.
    void UpdateButtons();

    /// @brief Update the list widget with the latest data.
    void UpdateListWidget();

    /// @brief Update tool tips.
    void UpdateToolTips();

    /// @brief  Handle to the Vertical PushButtons Layout of the dialog.
    /// @return pointer to the Vertical PushButtons Layout.
    QVBoxLayout* GetVerticalPushButtonsLayout();

    /// @brief  Handle to the Delete PushButton of the dialog.
    /// @return pointer to the Delete PushButton.
    QPushButton* GetDeletePushButton();

    /// @brief  Handle to the Item List Widget managed the dialog.
    /// @return pointer to the Item List Widget.
    QListWidget* GetItemsListWidget();

    QStringList items_list_;  ///< The list holding selected list items.
    QString     delimiter_;   ///< The delimiter used to split the ordered list.
    bool        editing_invalid_entry_ =
        false;  ///< Indicate whether editing an invalid entry. This means either the directory does not exist or a duplicate entry was entered.

private:
    /// @brief Connect the signals.
    void ConnectSignals();

    /// @brief  Find out if the Move up/down buttons need to be disabled.
    /// @return true if move up/down action buttons should be disabled, false otherwise
    bool ShouldDisableMoveUpDownButtons();

    /// @brief Set the button fonts.
    void SetButtonFonts();

    /// @brief Set button shortcuts.
    void SetButtonShortcuts();

    /// @brief Set the cursor to pointing hand cursor for various widgets.
    void SetCursor();

    /// @brief  Show a confirmation dialog with the given title, message, and Yes/No buttons.
    /// @return true if Yes button is clicked, false otherwise
    bool ShowConfirmationMessageBox();

    std::unique_ptr<Ui::OrderedListViewDialog> ui_;                ///< The Qt ui form.
    QIcon                                      window_icon_;       ///< Window icon of the dialog.
    std::unique_ptr<QAction>                   add_action_;        ///< Add button keyboard action.
    std::unique_ptr<QAction>                   delete_action_;     ///< Delete button keyboard action.
    std::unique_ptr<QAction>                   move_up_action_;    ///< Move up button keyboard action.
    std::unique_ptr<QAction>                   move_down_action_;  ///< Move down button keyboard action.
};
#endif  // #define QTCOMMON_CUSTOM_WIDGETS_ORDERED_LIST_DIALOG_H_
