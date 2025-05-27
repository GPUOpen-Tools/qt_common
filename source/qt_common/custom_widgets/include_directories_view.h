//=============================================================================
/// Copyright (c) 2025 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief  Header for our include directories view dialog
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_INCLUDE_DIRECTORIES_VIEW_H_
#define QTCOMMON_CUSTOM_WIDGETS_INCLUDE_DIRECTORIES_VIEW_H_

#include "ordered_list_dialog.h"

/// @brief Class that implements the include directories view
class IncludeDirectoriesView : public OrderedListDialog
{
    Q_OBJECT

public:
    /// @brief Constructor for IncludeDirectoriesView.
    /// @param [in] delimiter The delimiter used to split the ordered list managed by this class.
    /// @param [in] window_title The window title for this dialog.
    /// @param [in] window_icon The window icon for this dialog.
    /// @param [in] parent The parent widget.
    IncludeDirectoriesView(const QString& delimiter, const QString& window_title, const QIcon& window_icon = {}, QWidget* parent = nullptr);

    /// @brief Destructor.
    virtual ~IncludeDirectoriesView() = default;

    /// @brief Set a default folder path to be used by this dialog.
    /// @param [in] entries The default folder path as input.
    void SetDefaultFolderPath(std::string default_folder_path);

private slots:
    /// @brief Handler when the include file browse button is clicked.
    void HandleIncludeFileLocationBrowseButtonClick(bool /* checked */);

protected:
    /// @brief An overridden virtual responsible for determining if an edited list item is valid.
    /// @param [in] item The list widget item that changed.
    virtual void OnListItemChanged(QListWidgetItem* item) override;

private:
    /// @brief Connect the signals.
    void ConnectSignals();

    /// @brief Initialize the view and add a "Browse" button to browse for new directories to add.
    void InitializeBrowseButton();

    /// @brief Set the cursor to pointing hand cursor for various widgets.
    void SetCursor();

    /// @brief Set the button fonts.
    void SetButtonFonts();

    QPushButton* browse_push_button_  = nullptr;  ///< A "Browse" QPushButton used to browse new directories to add.
    std::string  default_folder_path_ = "./";     ///< The default folder path, to be set by the client of this dialog.
};
#endif  // QTCOMMON_CUSTOM_WIDGETS_INCLUDE_DIRECTORIES_VIEW_H_
