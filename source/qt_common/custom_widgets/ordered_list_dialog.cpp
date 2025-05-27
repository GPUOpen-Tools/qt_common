//=============================================================================
/// Copyright (c) 2025 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief  Implementation for our ordered list dialog
//=============================================================================
#include "ordered_list_dialog.h"
#include "ui_ordered_list_view_dialog.h"

#include <QAction>
#include <QMessageBox>

#include "common_definitions.h"

static const char* kStrIncludeDirDialogDeleteBoxTitle = "Delete confirmation";  ///< The dialog box title confirming entry deletion.
static const char* kStrIncludeDirDialogDeleteBoxMessage =
    "Are you sure you want to delete this entry?";  ///< The message displayed to the user confirming entry deletion.

OrderedListDialog::OrderedListDialog(const QString& delimiter, const QIcon& window_icon, QWidget* parent)
    : QDialog(parent)
    , delimiter_(delimiter)
    , ui_(std::make_unique<Ui::OrderedListViewDialog>())
    , window_icon_(window_icon)
{
    const int kWidgetFixedWidth  = 600;
    const int kWidgetFixedHeight = 150;

    // Verify that the given delimiter is valid.
    assert(!delimiter_.isEmpty());

    // Setup the UI.
    ui_->setupUi(this);

    if (!window_icon_.isNull())
    {
        // Set the window icon.
        setWindowIcon(window_icon_);
    }

    // Set the size of the window.
    QSize size;
    size.setWidth(kWidgetFixedWidth);
    size.setHeight(kWidgetFixedHeight);
    setMinimumSize(size);

    // Disable the help button in the title bar.
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    // Connect the signals.
    ConnectSignals();

    // Set the mouse cursor to the pointing hand cursor for various widgets.
    SetCursor();

    // Set the button fonts.
    SetButtonFonts();

    // Set push button shortcuts.
    SetButtonShortcuts();

    // Start off with various buttons disabled.
    ui_->moveUpPushButton->setEnabled(false);
    ui_->moveDownPushButton->setEnabled(false);
    ui_->editPushButton->setEnabled(false);
}

OrderedListDialog::~OrderedListDialog()
{
}

void OrderedListDialog::SetListItems(const QString& entries)
{
    // Clear any existing data.
    items_list_.clear();
    ui_->itemsList->clear();

    if (!entries.isEmpty())
    {
        // Process the incoming text.
        QStringList entry_list = entries.split(delimiter_);

        // Remove trailing and leading whitespace, and then only add non-empty strings.
        // This could happen if the user manually typed invalid entries.
        for (int i = 0; i < entry_list.count(); ++i)
        {
            QString item = entry_list[i].trimmed();

            if (item != "")
            {
                items_list_.append(item);
            }
        }

        // Update the list widget.
        UpdateListWidget();
    }

    // Insert an empty item to the list widget and select it.
    InsertBlankItem();
}

void OrderedListDialog::ConnectSignals()
{
    // Cancel button.
    [[maybe_unused]] bool is_connected = connect(ui_->cancelPushButton, &QPushButton::clicked, this, &OrderedListDialog::HandleExit);
    assert(is_connected);

    // Add new entry button.
    is_connected = connect(this->ui_->newPushButton, &QPushButton::clicked, this, &OrderedListDialog::HandleNewButtonClick);
    assert(is_connected);

    // Edit button.
    is_connected = connect(this->ui_->editPushButton, &QPushButton::clicked, this, &OrderedListDialog::HandleEditButtonClick);
    assert(is_connected);

    // "OK" button.
    is_connected = connect(this->ui_->okPushButton, &QPushButton::clicked, this, &OrderedListDialog::HandleOKButtonClick);
    assert(is_connected);

    // "Move up" button.
    is_connected = connect(this->ui_->moveUpPushButton, &QPushButton::clicked, this, &OrderedListDialog::HandleMoveUpButtonClick);
    assert(is_connected);

    // "Move down" button.
    is_connected = connect(this->ui_->moveDownPushButton, &QPushButton::clicked, this, &OrderedListDialog::HandleMoveDownButtonClick);
    assert(is_connected);

    // "Delete" button.
    is_connected = connect(this->ui_->deletePushButton, &QPushButton::clicked, this, &OrderedListDialog::HandleDeleteButtonClick);
    assert(is_connected);

    // Items tree widget.
    is_connected = connect(ui_->itemsList, &QListWidget::itemChanged, this, &OrderedListDialog::HandleListItemChanged);
    assert(is_connected);

    is_connected = connect(ui_->itemsList, &QListWidget::itemSelectionChanged, this, &OrderedListDialog::HandleListItemSelectionChanged);
    assert(is_connected);
}

void OrderedListDialog::SetCursor()
{
    // Set the cursor to pointing hand cursor.
    ui_->cancelPushButton->setCursor(Qt::PointingHandCursor);
    ui_->deletePushButton->setCursor(Qt::PointingHandCursor);
    ui_->moveDownPushButton->setCursor(Qt::PointingHandCursor);
    ui_->moveUpPushButton->setCursor(Qt::PointingHandCursor);
    ui_->newPushButton->setCursor(Qt::PointingHandCursor);
    ui_->okPushButton->setCursor(Qt::PointingHandCursor);
}

void OrderedListDialog::SetButtonFonts()
{
    // Create font.
    QFont font = ui_->deletePushButton->font();
    font.setPointSize(kButtonFontPointSize);

    // Update font size for all the buttons.
    ui_->cancelPushButton->setFont(font);
    ui_->deletePushButton->setFont(font);
    ui_->moveDownPushButton->setFont(font);
    ui_->moveUpPushButton->setFont(font);
    ui_->newPushButton->setFont(font);
    ui_->okPushButton->setFont(font);
}

void OrderedListDialog::HandleExit(bool /* checked */)
{
    // Clear any existing data.
    items_list_.clear();
    ui_->itemsList->clear();

    close();
}

void OrderedListDialog::HandleOKButtonClick(bool /* checked */)
{
    // Remove any empty entries.
    items_list_.removeAll("");

    // Emit a signal to indicate "OK" button clicked so the data can be saved.
    emit OKButtonClicked(items_list_);

    // Close the dialog.
    close();
}

void OrderedListDialog::UpdateListWidget()
{
    // Clear any existing data.
    ui_->itemsList->clear();

    int counter = 0;

    // Update the list widget.
    foreach (auto entry, items_list_)
    {
        QListWidgetItem* item = new QListWidgetItem;
        item->setText(entry);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui_->itemsList->insertItem(counter, item);
        counter++;
    }
}

void OrderedListDialog::HandleMoveDownButtonClick(bool /* checked */)
{
    // Get the current index.
    int current_index = ui_->itemsList->currentRow();

    // Do not do anything if this is the last item.
    if (current_index < ui_->itemsList->count() - 1)
    {
        // If the next item is empty, do not do anything.
        QString next_item = ui_->itemsList->item(current_index + 1)->text();
        if (!next_item.isEmpty())
        {
            QListWidgetItem* current_item = ui_->itemsList->takeItem(current_index);
            if (current_item != nullptr)
            {
                ui_->itemsList->insertItem(current_index + 1, current_item);
                ui_->itemsList->setCurrentRow(current_index + 1);

                // Update local data with this change.
                items_list_.swapItemsAt(current_index, current_index + 1);
            }
        }
    }

    // Update buttons.
    UpdateButtons();
}

void OrderedListDialog::HandleMoveUpButtonClick(bool /* checked */)
{
    // Get the current index.
    int current_index = ui_->itemsList->currentRow();

    // Make sure the index is not out of bounds.
    if (current_index < ui_->itemsList->count())
    {
        // Do not do anything if this is the first item, or the text field is empty.
        if (current_index > 0 && !ui_->itemsList->item(current_index)->text().isEmpty())
        {
            QListWidgetItem* current_item = ui_->itemsList->takeItem(current_index);
            if (current_item != nullptr)
            {
                ui_->itemsList->insertItem(current_index - 1, current_item);
                ui_->itemsList->setCurrentRow(current_index - 1);

                // Update local data with this change.
                items_list_.swapItemsAt(current_index - 1, current_index);
            }
        }
    }

    // Update buttons.
    UpdateButtons();
}

void OrderedListDialog::HandleDeleteButtonClick(bool /* checked */)
{
    // Display a confirmation message box.
    bool is_confirmation = ShowConfirmationMessageBox();

    if (is_confirmation)
    {
        // Remove the selected item from the list widget.
        int              current_index = ui_->itemsList->currentRow();
        QListWidgetItem* current_item  = ui_->itemsList->takeItem(current_index);

        // Remove the selected item from the string list.
        if (current_item != nullptr && current_index < items_list_.count())
        {
            items_list_.removeAt(current_index);

            // Update buttons.
            UpdateButtons();
        }

        // If this was the last item in the list widget,
        // add a blank place holder.
        if (ui_->itemsList->count() == 0)
        {
            // Insert an empty item to the list widget and highlight it.
            InsertBlankItem();
        }
    }
}

void OrderedListDialog::SetButtonShortcuts()
{
    // Add button keyboard shortcut.
    add_action_ = std::make_unique<QAction>(this);
    add_action_->setShortcut(QKeySequence(Qt::Key_Alt | Qt::Key_N));

    [[maybe_unused]] bool is_connected = connect(add_action_.get(), &QAction::triggered, this, &OrderedListDialog::HandleNewButtonClick);
    assert(is_connected);

    // Delete button keyboard shortcut.
    delete_action_ = std::make_unique<QAction>(this);
    delete_action_->setShortcut(QKeySequence(Qt::Key_Alt | Qt::Key_D));

    is_connected = connect(delete_action_.get(), &QAction::triggered, this, &OrderedListDialog::HandleDeleteButtonClick);
    assert(is_connected);

    // "Move up" button shortcut.
    move_up_action_ = std::make_unique<QAction>(this);
    move_up_action_->setShortcut(QKeySequence(Qt::Key_Alt | Qt::Key_U));

    is_connected = connect(move_up_action_.get(), &QAction::triggered, this, &OrderedListDialog::HandleMoveUpButtonClick);
    assert(is_connected);

    // "Move down" button shortcut.
    move_down_action_ = std::make_unique<QAction>(this);
    move_down_action_->setShortcut(QKeySequence(Qt::Key_Alt | Qt::Key_O));

    is_connected = connect(move_down_action_.get(), &QAction::triggered, this, &OrderedListDialog::HandleMoveDownButtonClick);
    assert(is_connected);
}

void OrderedListDialog::HandleListItemChanged(QListWidgetItem* item)
{
    OnListItemChanged(item);
}

void OrderedListDialog::HandleNewButtonClick(bool /* checked */)
{
    // Get the last item in the list widget.
    QListWidgetItem* item = nullptr;
    if (ui_->itemsList->count() > 0)
    {
        item = ui_->itemsList->item(ui_->itemsList->count() - 1);
    }
    if (item != nullptr && item->text().isEmpty())
    {
        item = ui_->itemsList->item(ui_->itemsList->count() - 1);
        ui_->itemsList->setCurrentItem(item);
        ui_->itemsList->editItem(item);
    }
    else
    {
        item = new QListWidgetItem;
        item->setText("");
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui_->itemsList->setCurrentItem(item);
        ui_->itemsList->insertItem(ui_->itemsList->count(), item);
        ui_->itemsList->editItem(item);
    }

    // Update buttons.
    UpdateButtons();
}

void OrderedListDialog::HandleEditButtonClick(bool /* checked */)
{
    // Get the current row.
    const int row = ui_->itemsList->currentRow();

    // Make the current row editable.
    QListWidgetItem* item = ui_->itemsList->item(row);
    ui_->itemsList->editItem(item);
}

void OrderedListDialog::UpdateButtons()
{
    // Enable/disable move up/down buttons.
    if (ShouldDisableMoveUpDownButtons())
    {
        ui_->moveUpPushButton->setEnabled(false);
        ui_->moveDownPushButton->setEnabled(false);
    }
    else
    {
        // Get the current row.
        const int current_row = ui_->itemsList->currentRow();

        // Set both up/down buttons to disabled for now.
        ui_->moveUpPushButton->setEnabled(false);
        ui_->moveDownPushButton->setEnabled(false);

        // Figure out which up/down buttons to enable.
        if (current_row > 0)
        {
            ui_->moveUpPushButton->setEnabled(true);
        }
        if (current_row >= 0 && current_row < items_list_.count() - 1)
        {
            ui_->moveDownPushButton->setEnabled(true);
        }
    }

    // Enable/Disable the Edit and Delete buttons.
    if (ui_->itemsList->count() == 0)
    {
        ui_->editPushButton->setEnabled(false);
        ui_->deletePushButton->setEnabled(false);
    }
    else
    {
        ui_->editPushButton->setEnabled(true);
        ui_->deletePushButton->setEnabled(true);
    }
}

bool OrderedListDialog::ShouldDisableMoveUpDownButtons()
{
    bool should_disable_buttons = false;

    if (ui_->itemsList->count() < 2)
    {
        should_disable_buttons = true;
    }
    else if (ui_->itemsList->count() == 2)
    {
        if (ui_->itemsList->item(1)->text().isEmpty())
        {
            should_disable_buttons = true;
        }
    }
    else if (ui_->itemsList->currentRow() >= 0 && ui_->itemsList->item(ui_->itemsList->currentRow())->text().isEmpty())
    {
        should_disable_buttons = true;
    }

    return should_disable_buttons;
}

void OrderedListDialog::UpdateToolTips()
{
    for (int i = 0; i < ui_->itemsList->count(); i++)
    {
        QListWidgetItem* item = ui_->itemsList->item(i);
        if (item != nullptr)
        {
            item->setToolTip(item->text());
        }
    }
}

void OrderedListDialog::InsertBlankItem()
{
    // Create a blank item and insert it in the list widget.
    QListWidgetItem* item = new QListWidgetItem();
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    ui_->itemsList->addItem(item);
    ui_->itemsList->setCurrentItem(item);
    ui_->itemsList->setFocus();
}

void OrderedListDialog::HandleListItemSelectionChanged()
{
    UpdateButtons();
}

QVBoxLayout* OrderedListDialog::GetVerticalPushButtonsLayout()
{
    return ui_->verticalPushButtonsLayout;
}

QPushButton* OrderedListDialog::GetDeletePushButton()
{
    return ui_->deletePushButton;
}

QListWidget* OrderedListDialog::GetItemsListWidget()
{
    return ui_->itemsList;
}

bool OrderedListDialog::ShowConfirmationMessageBox()
{
    QMessageBox confirmation_dialog(this);
    confirmation_dialog.setWindowIcon(window_icon_);
    confirmation_dialog.setWindowTitle(kStrIncludeDirDialogDeleteBoxTitle);
    confirmation_dialog.setText(kStrIncludeDirDialogDeleteBoxMessage);
    confirmation_dialog.setIcon(QMessageBox::Question);
    confirmation_dialog.setModal(true);
    confirmation_dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    // Set button cursor to pointing hand cursor.
    QAbstractButton* button = confirmation_dialog.button(QMessageBox::Button::Yes);
    if (button != nullptr)
    {
        button->setCursor(Qt::PointingHandCursor);
    }

    button = confirmation_dialog.button(QMessageBox::Button::No);
    if (button != nullptr)
    {
        button->setCursor(Qt::PointingHandCursor);
    }

    // Return true if the user clicked yes, otherwise return false.
    return (confirmation_dialog.exec() == QMessageBox::Yes);
}