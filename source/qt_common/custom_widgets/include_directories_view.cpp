//=============================================================================
/// Copyright (c) 2025 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief  Implementation for our include directories view dialog
//=============================================================================

#include "include_directories_view.h"

#include <algorithm>
#include <cassert>
#include <sstream>

#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "common_definitions.h"

static const char* kStrIncludeDirDialogBrowseButton       = "&Browse...";                           ///< Select include directories browse button string.
static const char* kStrIncludeDirDialogDirDoesNotExist    = "This directory does not exist.";       ///< Select include directory does not exist message string.
static const char* kStrIncludeDirDialogDirAlreadySelected = "This directory is already selected.";  ///< Select include directory already set message string.
static const char* kStrIncludeDirDialogSelectDirTitle     = "Select a directory";                   ///< Select include directories file dialog title string.

IncludeDirectoriesView::IncludeDirectoriesView(const QString& delimiter, const QString& window_title, const QIcon& window_icon, QWidget* parent)
    : OrderedListDialog(delimiter, window_icon, parent)
{
    // Initialize the browse button that gets inserted into the view.
    InitializeBrowseButton();

    // Set the window title.
    setWindowTitle(window_title);

    // Connect the signals.
    ConnectSignals();

    // Set the mouse cursor to the pointing hand cursor for various widgets.
    SetCursor();

    // Set the button fonts.
    SetButtonFonts();

    // Update various buttons.
    UpdateButtons();
}

void IncludeDirectoriesView::ConnectSignals()
{
    assert(browse_push_button_ != nullptr);

    // Browse new include directory button.
    [[maybe_unused]] bool is_connected =
        connect(browse_push_button_, &QPushButton::clicked, this, &IncludeDirectoriesView::HandleIncludeFileLocationBrowseButtonClick);
    assert(is_connected);
}

void IncludeDirectoriesView::InitializeBrowseButton()
{
    // Create a new button to browse for new directories.
    browse_push_button_ = new QPushButton(this);
    browse_push_button_->setText(kStrIncludeDirDialogBrowseButton);

    assert(browse_push_button_ != nullptr);
    if (browse_push_button_ != nullptr)
    {
        // This index specifies where the browse button will get inserted in the vertical layout of buttons in the dialog.
        static const int kBrowseButtonInsertionIndex = 2;

        // Add the new browse button to the view.
        QVBoxLayout* vertical_buttons_layout = GetVerticalPushButtonsLayout();
        assert(vertical_buttons_layout != nullptr);
        if (vertical_buttons_layout != nullptr)
        {
            vertical_buttons_layout->insertWidget(kBrowseButtonInsertionIndex, browse_push_button_);
        }
    }
}

void IncludeDirectoriesView::SetCursor()
{
    assert(browse_push_button_ != nullptr);
    if (browse_push_button_ != nullptr)
    {
        // Set the cursor to pointing hand cursor on the Browse button.
        browse_push_button_->setCursor(Qt::PointingHandCursor);
    }
}

void IncludeDirectoriesView::SetButtonFonts()
{
    // Create a font based on other QPushButtons in within the view.
    QFont font = GetDeletePushButton()->font();
    font.setPointSize(kButtonFontPointSize);

    assert(browse_push_button_ != nullptr);
    if (browse_push_button_ != nullptr)
    {
        // Update font size for all the buttons.
        browse_push_button_->setFont(font);
    }
}

void IncludeDirectoriesView::SetDefaultFolderPath(std::string default_folder_path)
{
    default_folder_path_ = default_folder_path;
}

void IncludeDirectoriesView::HandleIncludeFileLocationBrowseButtonClick(bool /* checked */)
{
    // Create a file chooser dialog.
    QFileDialog file_dialog;

    // Get the last entry from the directories list and open the dialog there.
    std::string latest_path = default_folder_path_;
    // If the user has an item selected, use that as starting path.
    // Otherwise use the last item in the list since it is probably most recent.
    QListWidgetItem* item = GetItemsListWidget()->currentItem();
    if (item != nullptr && !item->text().isEmpty() && QDir(item->text()).exists())
    {
        latest_path = item->text().toStdString();
    }
    else if (items_list_.size() > 0)
    {
        latest_path = items_list_.at(items_list_.size() - 1).toStdString();
    }

    bool should_show_find_directory_dialog = true;

    while (should_show_find_directory_dialog)
    {
        QString selected_directory =
            QFileDialog::getExistingDirectory(this, tr(kStrIncludeDirDialogSelectDirTitle), latest_path.c_str(), QFileDialog::ShowDirsOnly);

        // If the user did not select an entry, don't update anything, just exit the loop.
        if (selected_directory.isEmpty())
        {
            should_show_find_directory_dialog = false;
        }
        else
        {
            // If not a duplicate selection, update the list widget.
            if (!items_list_.contains(selected_directory))
            {
                item = GetItemsListWidget()->currentItem();
                if (item == nullptr)
                {
                    // There was no selected item,
                    // so make sure there is a final entry that is empty, and edit that.
                    if (GetItemsListWidget()->count() > 0)
                    {
                        item = GetItemsListWidget()->item(GetItemsListWidget()->count() - 1);
                    }

                    if (item == nullptr || !item->text().isEmpty())
                    {
                        // Add an empty row to the dialog
                        InsertBlankItem();
                        assert(GetItemsListWidget()->count() > 0);

                        // Use the empty row for the new item
                        item = GetItemsListWidget()->item(GetItemsListWidget()->count() - 1);
                    }
                }

                assert(item != nullptr);
                item->setText(selected_directory);

                // If the last item in the UI is no longer empty, add another item.
                item = GetItemsListWidget()->item(GetItemsListWidget()->count() - 1);
                if (item == nullptr || !item->text().isEmpty())
                {
                    // Add an empty row to the dialog
                    InsertBlankItem();
                }

                // Don't show the find directory dialog again.
                should_show_find_directory_dialog = false;
            }
            else
            {
                // Display an error message when a duplicate directory is selected.
                QMessageBox::critical(this, "Error", kStrIncludeDirDialogDirAlreadySelected);

                // Make the user try again.
                should_show_find_directory_dialog = true;
            }
        }
    }

    // Update move buttons.
    UpdateButtons();
}

void IncludeDirectoriesView::OnListItemChanged(QListWidgetItem* item)
{
    // Block signals from the list widget.
    GetItemsListWidget()->blockSignals(true);

    editing_invalid_entry_ = false;

    // Process the newly-entered data.
    if (item != nullptr)
    {
        QString new_directory = item->text();

        bool directory_exists      = QDir(new_directory).exists();
        bool directory_duplicate   = items_list_.contains(new_directory);
        bool directory_value_empty = new_directory.isEmpty();

        int item_row = GetItemsListWidget()->row(item);

        if (directory_value_empty && item_row != GetItemsListWidget()->count() - 1)
        {
            // The user has emptied out the entry, so delete it.
            // Simulate a click on the delete button to remove the entry from the UI.
            GetDeletePushButton()->click();
        }
        else
        {
            // If the new directory exists, and it is not a duplicate entry, update local data.
            if (!directory_exists || directory_duplicate)
            {
                // Display an error message box.
                if (!directory_exists)
                {
                    QMessageBox::critical(this, "Error", kStrIncludeDirDialogDirDoesNotExist);
                }
                else if (directory_duplicate)
                {
                    QMessageBox::critical(this, "Error", kStrIncludeDirDialogDirAlreadySelected);
                }

                editing_invalid_entry_ = true;
            }

            // Update local data.
            if (item_row < items_list_.count())
            {
                items_list_[item_row] = new_directory;
            }
            else
            {
                items_list_.append(new_directory);
            }
        }

        // Update tool tips.
        UpdateToolTips();

        // Unblock signals from the list widget.
        GetItemsListWidget()->blockSignals(false);

        // Update buttons.
        UpdateButtons();
    }
}
