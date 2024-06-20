//=============================================================================
// Copyright (c) 2022-2023 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Header for a shared isa widget.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_WIDGET_H_
#define QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_WIDGET_H_

#include <deque>
#include <list>

#include <QModelIndex>
#include <QTimer>
#include <QValidator>
#include <QWidget>

#include "shared_isa_item_model.h"
#include "shared_isa_proxy_model.h"
#include "shared_isa_tree_view.h"

namespace Ui
{
    class SharedIsaWidget;
}

/// @brief SharedIsaWidget is a composite widget that displays isa.
///        Isa is displayed in a tree view with auxiliary widgets to
///        show/hide individual tree columns and navigate between instructions.
class SharedIsaWidget final : public QWidget
{
    Q_OBJECT
public:
    using ExpandCollapseState = std::list<bool>;

    /// @brief Constructor; setup Qt ui.
    ///
    /// @param [in] parent The parent widget.
    explicit SharedIsaWidget(QWidget* parent = nullptr);

    /// @brief Destructor.
    virtual ~SharedIsaWidget();

    /// @brief Set an isa item model, navigation widget parent, optional tree view, and optional proxy model.
    ///
    /// Use this function to override the default models and views and use your own instead.
    ///
    /// @param [in] navigation_widget_parent The widget to set as a parent of the navigation widget's combo box.
    /// @param [in] isa_model                The isa item model.
    /// @param [in] isa_view                 The optional isa tree view.
    /// @param [in] proxy_model              The optional proxy model.
    void SetModelAndView(QWidget*             navigation_widget_parent,
                         SharedIsaItemModel*  isa_model,
                         SharedIsaTreeView*   isa_view    = nullptr,
                         SharedIsaProxyModel* proxy_model = nullptr);

    /// @brief Expand or collapse all isa blocks in the tree view in this widget.
    ///
    /// If collapsed_blocks is not nullptr, and expand is true, expand all blocks except the collapsed blocks.
    ///
    /// @param [in] expand                   true to expand all isa blocks, false to collapse them.
    /// @param [in] resize_contents          true to resize all columnts to fit their contents, false to skip this step.
    /// @param [in] collapsed_blocks         The code blocks to keep collapsed even when expanding, nullptr to skip this step.
    void ExpandCollapseAll(bool expand = true, bool resize_contents = true, std::deque<bool>* collapsed_blocks = nullptr);

    /// @brief Save the expand state of all code block nodes currently in the attached model.
    ///        The Shared Isa model does not filter rows so assume that the number of rows in the model
    ///        is the number of rows in the view.
    ///
    /// @return The expand state of all code blocks in the Isa tree view; true for each row that is expanded, and false for each row that is collapsed.
    ExpandCollapseState SaveExpandState();

    /// @brief Restore the expand state of all code block rows in the Shared Isa tree view.
    ///
    /// @param expand_collapse_state The true/false expand state of every code block currently in the model/view.
    void RestoreExpandState(ExpandCollapseState expand_collapse_state);

    /// @brief Updates which rows in the view have their first column spanned whenever the data in the shared isa model changes.
    void UpdateSpannedColumns();

    /// @brief Clear the history of the branch label navigation widget.
    void ClearHistory();

    /// @brief Set focus on to the go to line, line edit widget, in this widget.
    void SetFocusOnGoToLineWidget();

    /// @brief Set focus on to the line edit search widget in this widget.
    void SetFocusOnSearchWidget();

    /// @brief Update the maximum line number allowed in the 'Go To Line' line edit.
    ///
    /// @param [in] line_count The new maximum line count.
    void SetGoToLineValidatorLineCount(int line_count);

    /// @brief Navigate to the next branch label in the shared isa viewer navigation history.
    void BranchLabelNavigationForward();

    /// @brief Navigate to the previous branch label in the shared isa viewer navigation history.
    void BranchLabelNavigationBack();

    /// @brief Checks if the row of this item matches the search text.
    ///
    /// @param [in] index The index.
    ///
    /// @return true if the row matches the search text.
    bool DoesIndexMatchSearch(QModelIndex index);

public slots:

    /// @brief Search request; search the ISA model for text in line edit search and find all matches.
    void Search();

protected:
    /// @brief Override key press to support giving focus to the search line edit.
    ///
    /// @param [in] event The key event.
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

private slots:
    /// @brief Respond to a column checkbox being changed and toggle the visibility of its corresponding column.
    ///
    /// @param [in] checked true if the checkbox is checked and the column should be visible, false otherwise.
    void ShowHideColumnClicked(bool checked);

    /// @brief Respond to a change of text in the search line edit; restart the search timer.
    ///
    /// @param [in] text The new text in the search line edit.
    void SearchTextChanged(const QString& text);

    /// @brief Respond to return pressed in the search line edit; scroll the tree view to the next search match.
    void SearchEntered();

    /// @brief Respond to return pressed in the Go To Line line edit; scroll the tree view to the specified line number.
    void GoToLineEntered();

    /// @brief Respond to Viewing Options button press; toggle the visibility state of the Viewing Options.
    void ToggleViewingOptions();

    /// @brief Force the isa scroll bar to recalculate the relative positions of search match indices, in case
    ///        any previous indices were expanded or collapsed.
    ///
    /// @param [in] index The view index of the code block that was expanded or collapsed.
    void RefreshSearchMatchLineNumbers(const QModelIndex& index);

private:
    /// @brief Line validator class to restrict the 'Go to line...' input.
    class LineValidator final : public QValidator
    {
    public:
        /// @brief Constructor.
        ///
        /// @param [in] parent The parent object.
        LineValidator(QObject* parent = nullptr)
            : QValidator(parent)
            , line_count_(0)
        {
        }

        /// Destructor
        ~LineValidator()
        {
        }

        /// @brief Sets the line count for validator.
        ///
        /// @param [in] count The line count.
        void SetLineCount(int count)
        {
            line_count_ = count;
        }

    protected:
        /// @brief Overridden validate method.
        ///
        /// @param [in] input The input string.
        /// @param [in] pos   The input position.
        ///
        /// @return The input validation state.
        State validate(QString& input, int& pos) const Q_DECL_OVERRIDE
        {
            Q_UNUSED(pos);

            QValidator::State result = QValidator::Invalid;

            if (input.isEmpty())
            {
                result = QValidator::Acceptable;
            }
            else
            {
                bool success = false;

                int value = input.toInt(&success);

                if (success)
                {
                    if (value >= 0 && value <= line_count_)
                    {
                        result = QValidator::Acceptable;
                    }
                }
            }

            return result;
        }

    private:
        int line_count_;  ///< Line count cache.
    };

    Ui::SharedIsaWidget* ui_;           ///< The Qt ui form.
    SharedIsaProxyModel* proxy_model_;  ///< Internal proxy model to assist hiding columns.

    LineValidator*    go_to_line_validator_;     ///< Validate input to the 'Go To Line' line edit.
    QTimer            search_timer_;             ///< Search delay timer.
    QModelIndexList   matches_;                  ///< Cache of list of matches from find query.
    int               find_index_;               ///< Cache of current find selection index.
    bool              viewing_options_visible_;  ///< Visibilty state of the Viewing Options widget.
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_WIDGET_H_
