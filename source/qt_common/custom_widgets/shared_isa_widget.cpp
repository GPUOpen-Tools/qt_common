//=============================================================================
// Copyright (c) 2022-2024 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Shared isa widget implementation.
//=============================================================================

#include "shared_isa_widget.h"
#include "ui_shared_isa_widget.h"

#include <QCheckBox>
#include <QLayoutItem>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWidget>

static const int kSearchTimeout = 150;

/// @brief Compare 2 model indices in the ISA model; compare indices by their line # (row # but relative to entire shader, not just code block).
///
/// @param [in] lhs The left hand side model index.
/// @param [in] rhs The right hand side model index.
///
/// @return true if lhs is less than rhs, false otherwise.
static bool CompareModelIndices(const QModelIndex& lhs, const QModelIndex& rhs)
{
    const int lhs_line_number = lhs.siblingAtColumn(SharedIsaItemModel::kLineNumber).data(Qt::DisplayRole).toInt();
    const int rhs_line_number = rhs.siblingAtColumn(SharedIsaItemModel::kLineNumber).data(Qt::DisplayRole).toInt();

    return lhs_line_number < rhs_line_number;
}

SharedIsaWidget::SharedIsaWidget(QWidget* parent)
    : QWidget(parent)
    , ui_(new Ui::SharedIsaWidget)
    , proxy_model_(nullptr)
    , go_to_line_validator_(nullptr)
    , viewing_options_visible_(false)
{
    ui_->setupUi(this);

    connect(ui_->pc_address_, &QAbstractButton::clicked, this, &SharedIsaWidget::ShowHideColumnClicked);
    connect(ui_->opcode_, &QAbstractButton::clicked, this, &SharedIsaWidget::ShowHideColumnClicked);
    connect(ui_->operands_, &QAbstractButton::clicked, this, &SharedIsaWidget::ShowHideColumnClicked);
    connect(ui_->binary_representation_, &QAbstractButton::clicked, this, &SharedIsaWidget::ShowHideColumnClicked);

    connect(ui_->search_, &QLineEdit::textChanged, this, &SharedIsaWidget::SearchTextChanged);
    connect(ui_->search_, &QLineEdit::returnPressed, this, &SharedIsaWidget::SearchEntered);
    connect(&search_timer_, &QTimer::timeout, this, &SharedIsaWidget::Search);

    ui_->viewing_options_checkboxes_widget_->setVisible(false);

    ui_->viewing_options_combo_->InitSingleSelect(this, "Viewing Options", true);

    ui_->viewing_options_combo_->RemoveEventFilter();

    connect(ui_->viewing_options_combo_, &QPushButton::pressed, this, &SharedIsaWidget::ToggleViewingOptions);

    go_to_line_validator_ = new LineValidator(ui_->go_to_line_);
    ui_->go_to_line_->setValidator(go_to_line_validator_);

    // Make the 'go to line' line edit's style sheet match the 'search' line edit's style sheet.
    ui_->go_to_line_->setStyleSheet("QLineEdit {border: 1px solid gray;}");

    connect(ui_->go_to_line_, &QLineEdit::returnPressed, this, &SharedIsaWidget::GoToLineEntered);
}

SharedIsaWidget::~SharedIsaWidget()
{
    delete go_to_line_validator_;
}

void SharedIsaWidget::SetModelAndView(QWidget*             navigation_widget_parent,
                                      SharedIsaItemModel*  isa_item_model,
                                      SharedIsaTreeView*   isa_view,
                                      SharedIsaProxyModel* proxy_model)
{
    if (isa_view != nullptr)
    {
        // Find the existing SharedIsaTreeView and replace it.

        QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(this->layout());

        if (layout)
        {
            for (int i = 0; i < layout->count(); ++i)
            {
                QLayoutItem* layout_item = layout->itemAt(i);
                QWidget*     widget      = layout_item->widget();

                if (widget != nullptr && widget == ui_->isa_tree_view_)
                {
                    layout_item = layout->takeAt(i);
                    layout_item->widget()->deleteLater();
                    delete layout_item;

                    layout->insertWidget(i, isa_view, 1);
                    ui_->isa_tree_view_ = isa_view;
                    break;
                }
            }

            layout->invalidate();
        }
    }

    if (ui_->isa_tree_view_ != nullptr)
    {
        ui_->isa_tree_view_->RegisterIsaWidget(this);
    }

    // Attach a client's proxy or make the default one instead.
    if (proxy_model != nullptr)
    {
        proxy_model_ = proxy_model;
    }
    else
    {
        proxy_model_ = new SharedIsaProxyModel;
    }

    proxy_model_->setSourceModel(isa_item_model);
    ui_->isa_tree_view_->setModel(proxy_model_);

    // Pass a pointer to the SharedIsaPane down to the nav widget so it can render its combo box correctly.
    ui_->branch_label_navigation_->InitializeHistoryComboBox(navigation_widget_parent);

    // Connect the tree view to the nav widget to assist recording navigation history.

    connect(ui_->isa_tree_view_,
            &SharedIsaTreeView::ScrolledToBranchOrLabel,
            ui_->branch_label_navigation_,
            &SharedIsaBranchLabelNavigationWidget::AddBranchOrLabelToHistory);

    connect(
        ui_->branch_label_navigation_, &SharedIsaBranchLabelNavigationWidget::Navigate, ui_->isa_tree_view_, &SharedIsaTreeView::ReplayBranchOrLabelSelection);

    // Listen to isa tree expand/collapse of code block indicies to update the search match indices.
    connect(ui_->isa_tree_view_, &QTreeView::collapsed, this, &SharedIsaWidget::RefreshSearchMatchLineNumbers);
    connect(ui_->isa_tree_view_, &QTreeView::expanded, this, &SharedIsaWidget::RefreshSearchMatchLineNumbers);
}

void SharedIsaWidget::ExpandCollapseAll(bool expand, bool resize_contents, std::deque<bool>* collapsed_blocks)
{
    // Disconnect to prevent duplicate updates.
    disconnect(ui_->isa_tree_view_, &QTreeView::collapsed, this, &SharedIsaWidget::RefreshSearchMatchLineNumbers);
    disconnect(ui_->isa_tree_view_, &QTreeView::expanded, this, &SharedIsaWidget::RefreshSearchMatchLineNumbers);

    if (expand)
    {
        if (collapsed_blocks == nullptr)
        {
            ui_->isa_tree_view_->expandAll();
        }
        else
        {
            SharedIsaItemModel* source_model = nullptr;

            if (proxy_model_ != nullptr)
            {
                source_model = qobject_cast<SharedIsaItemModel*>(proxy_model_->sourceModel());
            }
            else
            {
                source_model = qobject_cast<SharedIsaItemModel*>(ui_->isa_tree_view_->model());
            }

            if (source_model != nullptr)
            {
                for (int i = 0; i < source_model->rowCount(); i++)
                {
                    const QModelIndex code_block_source_index = source_model->index(i, SharedIsaItemModel::kLineNumber);
                    const QModelIndex code_block_proxy_index =
                        proxy_model_ != nullptr ? proxy_model_->mapFromSource(code_block_source_index) : code_block_source_index;

                    const bool is_block_collapsed = collapsed_blocks->at(i);

                    ui_->isa_tree_view_->setExpanded(code_block_proxy_index, !is_block_collapsed);
                }
            }
        }

        const QAbstractItemModel* model = ui_->isa_tree_view_->model();
        if (model != nullptr && resize_contents)
        {
            for (int i = 0; i < model->columnCount(); i++)
            {
                ui_->isa_tree_view_->resizeColumnToContents(i);
            }
        }
    }
    else
    {
        ui_->isa_tree_view_->collapseAll();
    }

    RefreshSearchMatchLineNumbers(QModelIndex());

    // Reconnect to resume normal updates.
    connect(ui_->isa_tree_view_, &QTreeView::collapsed, this, &SharedIsaWidget::RefreshSearchMatchLineNumbers);
    connect(ui_->isa_tree_view_, &QTreeView::expanded, this, &SharedIsaWidget::RefreshSearchMatchLineNumbers);
}

SharedIsaWidget::ExpandCollapseState SharedIsaWidget::SaveExpandState()
{
    const SharedIsaItemModel* source_model       = qobject_cast<SharedIsaItemModel*>(proxy_model_->sourceModel());
    const int                 number_code_blocks = source_model->rowCount();

    ExpandCollapseState expand_collapse_state;

    for (int i = 0; i < number_code_blocks; i++)
    {
        const QModelIndex code_block_line_number_source_index = source_model->index(i, SharedIsaItemModel::kLineNumber);
        const QModelIndex code_block_line_number_proxy_index  = proxy_model_->mapFromSource(code_block_line_number_source_index);

        const bool is_code_block_expanded = ui_->isa_tree_view_->isExpanded(code_block_line_number_proxy_index);

        expand_collapse_state.emplace_back(is_code_block_expanded);
    }

    return expand_collapse_state;
}

void SharedIsaWidget::RestoreExpandState(ExpandCollapseState expand_collapse_state)
{
    const SharedIsaItemModel* source_model       = qobject_cast<SharedIsaItemModel*>(proxy_model_->sourceModel());
    const int                 number_code_blocks = source_model->rowCount();

    if (number_code_blocks != static_cast<int>(expand_collapse_state.size()))
    {
        return;
    }

    // Disconnect to prevent duplicate updates.
    disconnect(ui_->isa_tree_view_, &QTreeView::collapsed, this, &SharedIsaWidget::RefreshSearchMatchLineNumbers);
    disconnect(ui_->isa_tree_view_, &QTreeView::expanded, this, &SharedIsaWidget::RefreshSearchMatchLineNumbers);

    int code_block_row = 0;

    for (const bool is_code_block_expanded : expand_collapse_state)
    {
        const QModelIndex code_block_line_number_source_index = source_model->index(code_block_row, SharedIsaItemModel::kLineNumber);
        const QModelIndex code_block_line_number_proxy_index  = proxy_model_->mapFromSource(code_block_line_number_source_index);

        ui_->isa_tree_view_->setExpanded(code_block_line_number_proxy_index, is_code_block_expanded);

        code_block_row++;
    }

    RefreshSearchMatchLineNumbers(QModelIndex());

    // Reconnect to resume normal updates.
    connect(ui_->isa_tree_view_, &QTreeView::collapsed, this, &SharedIsaWidget::RefreshSearchMatchLineNumbers);
    connect(ui_->isa_tree_view_, &QTreeView::expanded, this, &SharedIsaWidget::RefreshSearchMatchLineNumbers);
}

void SharedIsaWidget::UpdateSpannedColumns()
{
    const QAbstractItemModel* source_model = proxy_model_->sourceModel();

    for (int i = 0; i < source_model->rowCount(); i++)
    {
        const int proxy_row = proxy_model_->mapFromSource(source_model->index(i, SharedIsaItemModel::kOpCode)).row();

        // All parent labels (code blocks or comments) should span across columns.
        ui_->isa_tree_view_->setFirstColumnSpanned(proxy_row, QModelIndex(), true);

        const QModelIndex source_parent_index = source_model->index(i, SharedIsaItemModel::kLineNumber);

        for (int j = 0; j < source_model->rowCount(source_parent_index); j++)
        {
            QModelIndex source_child_index = source_model->index(j, SharedIsaItemModel::kOpCode, source_parent_index);
            const auto  row_type           = qvariant_cast<SharedIsaItemModel::RowType>(source_child_index.data(SharedIsaItemModel::kRowTypeRole));
            bool        spanned            = false;

            if (row_type == SharedIsaItemModel::RowType::kComment)
            {
                spanned = true;
            }

            const int proxy_child_row = proxy_model_->mapFromSource(source_child_index).row();

            // Child comments should span across columns.
            ui_->isa_tree_view_->setFirstColumnSpanned(proxy_child_row, proxy_model_->mapFromSource(source_parent_index), spanned);
        }
    }
    ui_->isa_tree_view_->ClearLastPinnedndex();
}

void SharedIsaWidget::ClearHistory()
{
    ui_->branch_label_navigation_->ClearHistory();
}

void SharedIsaWidget::SetFocusOnGoToLineWidget()
{
    ui_->go_to_line_->setFocus();
}

void SharedIsaWidget::SetFocusOnSearchWidget()
{
    ui_->search_->setFocus();
}

void SharedIsaWidget::SetGoToLineValidatorLineCount(int line_count)
{
    ui_->go_to_line_->clear();
    go_to_line_validator_->SetLineCount(line_count);
}

void SharedIsaWidget::Search()
{
    ui_->search_results_->setText("No results");

    matches_.clear();

    std::set<QModelIndex> match_source_indices;
    ui_->isa_tree_view_->SetSearchMatchLineNumbers(match_source_indices);

    auto text = ui_->search_->text();

    if (!text.isEmpty())
    {
        ui_->isa_tree_view_->selectionModel()->clearSelection();

        SharedIsaItemModel* source_model = qobject_cast<SharedIsaItemModel*>(proxy_model_->sourceModel());

        if (source_model != nullptr)
        {
            // Search all columns (except for line number) for matches.
            for (int col = (int)SharedIsaItemModel::kLineNumber + 1; col < proxy_model_->columnCount(); col++)
            {
                QModelIndex     column_index   = proxy_model_->index(0, col);
                QModelIndexList column_matches = proxy_model_->match(column_index, Qt::DisplayRole, text, -1, Qt::MatchContains | Qt::MatchRecursive);

                // Store these into matches_, but use the first column to avoid duplicates.
                for (QModelIndex index : column_matches)
                {
                    matches_ += index.siblingAtColumn(SharedIsaItemModel::kLineNumber);
                }
            }

            // Sort and uniquify.
            std::sort(matches_.begin(), matches_.end(), CompareModelIndices);
            matches_.erase(std::unique(matches_.begin(), matches_.end()), matches_.end());

            if (!matches_.isEmpty())
            {
                find_index_ = 0;

                ui_->search_results_->setText(QString("%1 of %2").arg(find_index_ + 1).arg(matches_.size()));

                const QModelIndex& view_index = matches_.at(find_index_);

                QModelIndex source_index = proxy_model_->mapToSource(view_index);
                ui_->isa_tree_view_->ScrollToBranchLabelIndex(source_index, false);

                ui_->isa_tree_view_->selectionModel()->select(view_index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
            }

            for (const QModelIndex& match_view_index : matches_)
            {
                QModelIndex match_source_index = proxy_model_->mapToSource(match_view_index);
                match_source_indices.emplace(match_source_index);
            }

            ui_->isa_tree_view_->SetSearchMatchLineNumbers(match_source_indices);
        }
    }

    search_timer_.stop();

    // Update the isa tree and its scroll bar.
    ui_->isa_tree_view_->viewport()->update();
    ui_->isa_tree_view_->verticalScrollBar()->update();
}

void SharedIsaWidget::keyPressEvent(QKeyEvent* event)
{
    bool event_handled = false;

    // The keyboard modifiers are different for PC/Linux vs macOS
#ifdef Q_OS_MACOS
    if (event->key() == Qt::Key_F && (event->modifiers() & Qt::MetaModifier))
#else
    if (event->key() == Qt::Key_F && (event->modifiers() & Qt::ControlModifier))
#endif  // Q_OS_MACOS
    {
        ui_->search_->setFocus();

        event->accept();
        event_handled = true;
    }

    if (!event_handled)
    {
        QWidget::keyPressEvent(event);
    }
}

void SharedIsaWidget::ShowHideColumnClicked(bool checked)
{
    const QObject* sender = this->sender();

    QHeaderView* header = ui_->isa_tree_view_->header();

    uint32_t source_column_index = SharedIsaItemModel::kColumnCount;

    if (sender == ui_->pc_address_)
    {
        source_column_index = SharedIsaItemModel::kPcAddress;
    }
    else if (sender == ui_->opcode_)
    {
        source_column_index = SharedIsaItemModel::kOpCode;
    }
    else if (sender == ui_->operands_)
    {
        source_column_index = SharedIsaItemModel::kOperands;
    }
    else if (sender == ui_->binary_representation_)
    {
        source_column_index = SharedIsaItemModel::kBinaryRepresentation;
    }

    if (source_column_index != SharedIsaItemModel::kColumnCount)
    {
        int proxy_index  = proxy_model_->mapFromSource(proxy_model_->sourceModel()->index(0, source_column_index)).column();
        int visual_index = header->visualIndex(proxy_index);

        proxy_model_->SetColumnVisibility(source_column_index, checked, header);

        if (checked)
        {
            ui_->isa_tree_view_->resizeColumnToContents(proxy_model_->mapFromSource(proxy_model_->sourceModel()->index(0, source_column_index)).column());
        }
        else
        {
            // If the last column was just removed, resize the column with the next logical index to prevent a bug where it gets too large.
            if (visual_index == proxy_model_->columnCount())
            {
                // Check if there was a next logical index.
                if (header->visualIndex(proxy_index) != -1)
                {
                    ui_->isa_tree_view_->resizeColumnToContents(proxy_index);
                }
            }
        }
    }

    // If there is only one column being shown, we need to disable the checkbox for that column.
    // Otherwise, the user could hide the last column, and the tree would not be useful.
    uint32_t                check_count  = 0;
    QCheckBox*              last_checked = nullptr;
    const QList<QCheckBox*> children     = ui_->viewing_options_checkboxes_widget_->findChildren<QCheckBox*>();

    for (auto& child : children)
    {
        if (child->isChecked())
        {
            check_count++;
            last_checked = child;
        }
        child->setEnabled(true);
    }

    if (check_count == 1)
    {
        last_checked->setEnabled(false);
    }
}

bool SharedIsaWidget::DoesIndexMatchSearch(QModelIndex index)
{
    QModelIndex sibling = index.siblingAtColumn(SharedIsaItemModel::kLineNumber);
    return matches_.contains(sibling);
}

void SharedIsaWidget::SearchTextChanged(const QString& text)
{
    Q_UNUSED(text);

    search_timer_.start(kSearchTimeout);
}

void SharedIsaWidget::SearchEntered()
{
    if (!matches_.isEmpty())
    {
        find_index_++;
        if (find_index_ >= matches_.size())
        {
            find_index_ = 0;
        }

        ui_->search_results_->setText(QString("%1 of %2").arg(find_index_ + 1).arg(matches_.size()));

        const QModelIndex& view_index = matches_.at(find_index_);

        QModelIndex source_index = proxy_model_->mapToSource(view_index);
        ui_->isa_tree_view_->ScrollToBranchLabelIndex(source_index, false);

        ui_->isa_tree_view_->selectionModel()->select(view_index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

        // Make sure the tree's custom row background color repaints.
        ui_->isa_tree_view_->viewport()->update();
    }
}

void SharedIsaWidget::GoToLineEntered()
{
    SharedIsaItemModel* source_model = qobject_cast<SharedIsaItemModel*>(proxy_model_->sourceModel());

    if (source_model == nullptr)
    {
        return;
    }

    const int go_to_line_number = ui_->go_to_line_->text().toInt();

    const QModelIndex source_go_to_index = source_model->GetLineNumberModelIndex(go_to_line_number);

    ui_->isa_tree_view_->ScrollToBranchLabelIndex(source_go_to_index, false);
}

void SharedIsaWidget::ToggleViewingOptions()
{
    ui_->viewing_options_combo_->ToggleDirection();

    viewing_options_visible_ = !viewing_options_visible_;

    ui_->viewing_options_checkboxes_widget_->setVisible(viewing_options_visible_);
}

void SharedIsaWidget::RefreshSearchMatchLineNumbers(const QModelIndex& index)
{
    Q_UNUSED(index);

    std::set<QModelIndex> match_source_indices;

    for (const QModelIndex& match_proxy_index : matches_)
    {
        QModelIndex match_source_index = proxy_model_->mapToSource(match_proxy_index);
        match_source_indices.emplace(match_source_index);
    }

    ui_->isa_tree_view_->SetSearchMatchLineNumbers(match_source_indices);
}

void SharedIsaWidget::BranchLabelNavigationForward()
{
    if (ui_->branch_label_navigation_->ForwardButton().isEnabled())
    {
        ui_->branch_label_navigation_->ForwardPressed();
    }
}

void SharedIsaWidget::BranchLabelNavigationBack()
{
    if (ui_->branch_label_navigation_->BackButton().isEnabled())
    {
        ui_->branch_label_navigation_->BackPressed();
    }
}
