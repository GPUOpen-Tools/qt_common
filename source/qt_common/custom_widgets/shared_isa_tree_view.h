//=============================================================================
// Copyright (c) 2022-2023 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Header for a shared isa tree view.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_TREE_VIEW_H_
#define QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_TREE_VIEW_H_

#include <QKeyEvent>

#include "scaled_tree_view.h"
#include "shared_isa_item_model.h"
#include "shared_isa_vertical_scroll_bar.h"

// Forward declaration to prevent a circular dependency.
class SharedIsaWidget;

/// @brief SharedIsaTreeView is a tree view intended to be attached to a SharedIsaItemModel
///        to display isa in a tree structure. It instantiates and uses a SharedIsaItemDelegate
///        to do any custom rendering and handle user interaction.
class SharedIsaTreeView : public ScaledTreeView
{
    Q_OBJECT

public:

    /// @brief Constructor; set default properties on tree and header, create delegate and scroll bar.
    ///
    /// @param [in] parent The parent widget.
    explicit SharedIsaTreeView(QWidget* parent = nullptr);

    /// @brief Destructor.
    virtual ~SharedIsaTreeView();

    /// @brief Set the line # of a hot spot to paint a red rectangle in the scroll bar.
    ///        Use an instruction's source index to get its relative line number.
    ///
    /// @param [in] instruction_source_index The source index of the hot spot instruction.
    void SetHotSpotLineNumber(const QModelIndex instruction_source_index);

    /// @brief Set the line #(s) of text search matches to paint a purple rectangle(s) in the scroll bar.
    ///        Use instructions' source indices to get their relative line numbers.
    ///
    /// @param [in] source_indices The source indices of the text search matches.
    void SetSearchMatchLineNumbers(const std::set<QModelIndex>& source_indices);

    /// @brief Show a popup menu that scrolls to a branch label instruction after pressing a menu action.
    ///
    /// @param source_indices  [in] Source model indices of branch instructions.
    /// @param global_position [in] The global position to show the menu at.
    void ShowBranchInstructionsMenu(QVector<QModelIndex> source_indices, QPoint global_position);

    /// @brief Respond to a request and scroll this tree to the given source model index.
    ///
    /// @param [in] source_index The source model index to scroll to.
    /// @param [in] record       true to record the scroll into navigation history, false to skip recording.
    void ScrollToBranchLabelIndex(const QModelIndex source_index, bool record = true);

    /// @brief Toggles the copy_line_numbers_ variable true and false. Used to know if line number text should be included when copying isa text.
    void ToggleCopyLineNumbers();

    /// @brief Saves a link (pointer) to the ISA widget for use later.
    ///
    /// @param [in] widget The pointer to the SharedIsaWidget.
    void RegisterIsaWidget(SharedIsaWidget* widget);

public slots:

    /// @brief Scroll to a branch or label but do not re-record the entry into history.
    ///
    /// @param [in] branch_label_source_index The source model index to scroll to.
    void ReplayBranchOrLabelSelection(QModelIndex branch_label_source_index);

signals:
    /// @brief Listeners can use this signal to respond to a branch or label that was scrolled to.
    ///
    /// @param [in] branch_label_source_index The source model index that was scrolled to.
    void ScrolledToBranchOrLabel(QModelIndex branch_label_source_index);

protected:
    /// @brief Override drawRow to manually paint alternating background colors to assist painting labels and comments across columns.
    ///
    /// In order to paint code block labels and comments such that they span multiple columns, we have to manually paint the alternating background color in the tree.
    /// If we let Qt paint the background color, it will paint over our attempt to span multiple columns.
    ///
    /// @param [in] painter The painter.
    /// @param [in] option  The style options.
    /// @param [in] index   The index.
    virtual void drawRow(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;

    /// @brief Override key press to support copy and paste.
    ///
    /// @param [in] event The key event.
    virtual void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

private:
    /// @brief SetAutoScrollObject is a helper class to help temporarily enable a QTreeView's autoScroll property.
    class SetAutoScrollObject
    {
    public:
        /// @brief Explicit constructor.
        ///
        /// @param [in] tree_view The tree view to modify autoScroll for.
        explicit SetAutoScrollObject(QTreeView* tree_view)
        {
            auto_scroll_enabled_ = false;
            tree_view_           = tree_view;
        }

        /// @brief Destructor; turn autoScroll off again if this class enabled it.
        ~SetAutoScrollObject()
        {
            if (auto_scroll_enabled_ && tree_view_ != nullptr)
            {
                tree_view_->setAutoScroll(!auto_scroll_enabled_);
            }
        }

        /// @brief Temporarily enable autoScroll for the attached QTreeView.
        void EnableAutoScroll()
        {
            auto_scroll_enabled_ = true;

            if (tree_view_ != nullptr)
            {
                tree_view_->setAutoScroll(auto_scroll_enabled_);
            }
        }

    private:
        QTreeView* tree_view_;            ///< The attached QTreeView.
        bool       auto_scroll_enabled_;  ///< autoScroll status.

        // Remove constructors.
        SetAutoScrollObject(SetAutoScrollObject& rhs)  = delete;
        SetAutoScrollObject(SetAutoScrollObject&& rhs) = delete;

        // Remove assignment operators.
        SetAutoScrollObject& operator=(SetAutoScrollObject& rhs)  = delete;
        SetAutoScrollObject& operator=(SetAutoScrollObject&& rhs) = delete;
    };

    /// @brief Emit a model data changed signal when the scroll bar is scrolled
    ///        in order to custom render the 0th row in this tree when necessary.
    ///
    /// @param [in] value The new value of the scroll bar.
    void ScrollBarScrolled(int value);

    SharedIsaWidget*            shared_isa_widget_;      ///< The Isa widget.
    SharedIsaVerticalScrollBar* shared_isa_scroll_bar_;  ///< Scroll bar to paint red and purple rectangles corresponding to hot spots and text search matches.
    bool                        copy_line_numbers_;      ///< Whether the line number text is to be included when copying isa text. True by default.
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_TREE_VIEW_H_
