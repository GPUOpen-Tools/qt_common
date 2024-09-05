//=============================================================================
// Copyright (c) 2022-2024 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Shared isa tree view implementation.
//=============================================================================

#include "shared_isa_tree_view.h"

#include <QApplication>
#include <QClipboard>
#include <QFontDatabase>
#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QScrollBar>
#include <QSortFilterProxyModel>
#include <algorithm>
#include <utility>

#include "common_definitions.h"
#include "qt_util.h"
#include "shared_isa_item_delegate.h"
#include "shared_isa_widget.h"

/// struct containing info needed to sort model indices.
/// The y position is used for sorting the indices vertically by rows, and the visual column for sorting indices horizontally.
/// If the index is a comment it needs to be placed before all other columns except for line number.
struct CompareIndexInfo
{
    QModelIndex source_index;   ///< The source model index.
    int         visual_column;  ///< The order that the index's column appears visually on the table.
    int         y_pos;          ///< The y pixel coordinate of the index.
};

/// @brief Compare function that sorts model indices by y position and visual column index order.
///
/// Also makes sure comments always come before all other columns except for line number.
///
/// @param lhs The compare info for the left hand side index.
/// @param rhs The compare info for the right hand side model index.
///
/// @return true if the left model index is less than the right model index, false otherwise.
static bool CompareModelIndices(const CompareIndexInfo& lhs, const CompareIndexInfo& rhs)
{
    if (lhs.y_pos == rhs.y_pos)
    {
        const QModelIndex lhs_index = lhs.source_index;
        const QModelIndex rhs_index = rhs.source_index;

        // Put comments in the first row.
        if (lhs_index.data(SharedIsaItemModel::kRowTypeRole).value<SharedIsaItemModel::RowType>() == SharedIsaItemModel::RowType::kComment)
        {
            // Put the opcode column before all other columns except for line number for comments rows.
            if (lhs_index.column() == SharedIsaItemModel::Columns::kOpCode)
            {
                return rhs_index.column() != SharedIsaItemModel::Columns::kLineNumber;
            }
            else if (rhs_index.column() == SharedIsaItemModel::Columns::kOpCode)
            {
                return lhs_index.column() == SharedIsaItemModel::Columns::kLineNumber;
            }
        }

        return lhs.visual_column < rhs.visual_column;
    }

    return lhs.y_pos < rhs.y_pos;
}

SharedIsaTreeView::SharedIsaTreeView(QWidget* parent)
    : ScaledTreeView(parent)
    , shared_isa_widget_(nullptr)
    , shared_isa_scroll_bar_(nullptr)
    , copy_line_numbers_(true)
    , last_pinned_row_(std::pair<int, int>(-1, -1))
    , paint_column_separators_(true)
{
    setObjectName("isa_tree_view_");

    header()->setSectionResizeMode(QHeaderView::ResizeMode::Interactive);
    header()->setResizeContentsPrecision(ScaledHeaderView::kResizeContentsPrecision_AllRows);
    header()->setSectionsMovable(true);

    connect(header(), &QHeaderView::sectionResized, this, [this](int local_index, int old_size, int new_size) {
        Q_UNUSED(local_index);
        Q_UNUSED(old_size);
        Q_UNUSED(new_size);

        // Make sure any custom painting of code block labels or comments gets repainted.
        viewport()->update();
    });

    setUniformRowHeights(true);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setMouseTracking(true);

    // Disable autoscroll to mitigate scrolling horizontally when clicking near the edge of a view.
    // Turn it on temporarily in order to allow scrolling vertically using keybinds.
    setAutoScroll(false);

    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    SharedIsaItemDelegate* shared_isa_item_delegate = new SharedIsaItemDelegate(this);
    setItemDelegate(shared_isa_item_delegate);

    // Allow contiguous selection per rows.
    setSelectionMode(QAbstractItemView::SelectionMode::ContiguousSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    // Disable alternating row colors and have this view paint them manually instead.
    // The default alternating row color behavior would otherwise paint over our attempt to let labels and comments span columns.
    setAlternatingRowColors(false);

    // Scroll bar to show hot spots and text search matches.
    shared_isa_scroll_bar_ = new SharedIsaVerticalScrollBar(this);
    setVerticalScrollBar(shared_isa_scroll_bar_);
    connect(shared_isa_scroll_bar_, &QScrollBar::valueChanged, this, &SharedIsaTreeView::ScrollBarScrolled);
}

SharedIsaTreeView::~SharedIsaTreeView()
{
}

void SharedIsaTreeView::SetHotSpotLineNumber(const QModelIndex instruction_source_index)
{
    if (!instruction_source_index.parent().isValid())
    {
        // Should be an instruction index.
        shared_isa_scroll_bar_->SetHotSpotLineNumber(-1);
        return;
    }

    int                          line_number = 0;
    const QSortFilterProxyModel* proxy_model = qobject_cast<QSortFilterProxyModel*>(model());

    // Get the relative line number of the requested hot spot instruction, accounting for the
    // expand/collapse state of previous code blocks.

    if (proxy_model != nullptr)
    {
        const QModelIndex instruction_proxy_index = proxy_model->mapFromSource(instruction_source_index);
        const QModelIndex code_block_proxy_index  = proxy_model->mapFromSource(instruction_source_index.parent());

        for (int i = 0; i < code_block_proxy_index.row(); i++)
        {
            line_number++;  // +1 for previous code block's line number.

            const QModelIndex next_code_block_index = proxy_model->index(i, SharedIsaItemModel::Columns::kLineNumber);

            // Add previous code block instruction count if previous code block is expanded.
            if (isExpanded(next_code_block_index))
            {
                line_number += proxy_model->rowCount(next_code_block_index);
            }
        }

        line_number++;  // +1 for hot spot code block's line number.

        // Add the hot spot instruction's index if its code block is expanded.
        if (isExpanded(code_block_proxy_index))
        {
            line_number += instruction_proxy_index.row();
        }
    }

    shared_isa_scroll_bar_->SetHotSpotLineNumber(line_number);
}

void SharedIsaTreeView::SetSearchMatchLineNumbers(const std::set<QModelIndex>& source_indices)
{
    std::set<int> line_numbers;

    const QSortFilterProxyModel* proxy_model = qobject_cast<QSortFilterProxyModel*>(model());

    for (const auto& source_index : source_indices)
    {
        int line_number = 0;

        QModelIndex proxy_index        = source_index;
        QModelIndex proxy_index_parent = source_index.parent();

        // Get the relative line number of the requested search match index, accounting for the
        // expand/collapse state of previous code blocks.

        if (proxy_model != nullptr)
        {
            proxy_index = proxy_model->mapFromSource(proxy_index);

            int row = 0;

            if (proxy_index_parent.isValid())
            {
                // Search match matches an instruction.

                proxy_index_parent = proxy_model->mapFromSource(proxy_index_parent);

                row = proxy_index_parent.row();
            }
            else
            {
                // Search match matches a code block.

                row = proxy_index.row();
            }

            for (int i = 0; i < row; i++)
            {
                line_number++;  // +1 for previous code block's line number.

                const QModelIndex next_code_block_index = proxy_model->index(i, SharedIsaItemModel::Columns::kLineNumber);

                // Add previous code block instruction count if previous code block is expanded.
                if (isExpanded(next_code_block_index))
                {
                    line_number += proxy_model->rowCount(next_code_block_index);
                }
            }

            line_number++;  // +1 for search match code block's line number.

            // If a search match matches an instruction (not a code block), and its parent code block is expanded,
            // add the instruction's index too.
            if (proxy_index_parent.isValid() && isExpanded(proxy_index_parent))
            {
                line_number += proxy_index.row();
            }

            line_numbers.emplace(line_number);
        }
    }

    shared_isa_scroll_bar_->SetSearchMatchLineNumbers(line_numbers);
}

void SharedIsaTreeView::ShowBranchInstructionsMenu(QVector<QModelIndex> source_indices, QPoint global_position)
{
    QMenu branch_instruction_menu(this);

    QMap<QString, QModelIndex> action_to_index_map;

    for (const auto& source_index : source_indices)
    {
        QString line_number        = source_index.siblingAtColumn(SharedIsaItemModel::kLineNumber).data().toString();
        QString branch_instruction = source_index.siblingAtColumn(SharedIsaItemModel::kOpCode).data().toString();
        QString menu_action_text   = line_number + ": " + branch_instruction;

        branch_instruction_menu.addAction(menu_action_text);

        action_to_index_map[menu_action_text] = source_index;
    }

    const QAction* result_action = branch_instruction_menu.exec(global_position);

    if (result_action != nullptr && action_to_index_map.find(result_action->text()) != action_to_index_map.end())
    {
        const QModelIndex& source_index = action_to_index_map.value(result_action->text());

        ScrollToBranchLabelIndex(source_index);
    }

    setCursor(Qt::ArrowCursor);
}

void SharedIsaTreeView::ScrollToBranchLabelIndex(const QModelIndex source_index, bool record)
{
    const QSortFilterProxyModel* isa_tree_proxy_model = qobject_cast<const QSortFilterProxyModel*>(this->model());

    QModelIndex isa_tree_view_index = source_index;

    if (isa_tree_proxy_model != nullptr)
    {
        isa_tree_view_index = isa_tree_proxy_model->mapFromSource(isa_tree_view_index);
    }

    isa_tree_view_index = isa_tree_view_index.siblingAtColumn(SharedIsaItemModel::kLineNumber);

    scrollTo(isa_tree_view_index, QAbstractItemView::ScrollHint::PositionAtCenter);

    selectionModel()->setCurrentIndex(isa_tree_view_index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

    // Make sure the custom row background color repaints.
    viewport()->update();

    if (record)
    {
        emit ScrolledToBranchOrLabel(source_index);
    }
}

void SharedIsaTreeView::ReplayBranchOrLabelSelection(QModelIndex branch_label_source_index)
{
    ScrollToBranchLabelIndex(branch_label_source_index, false);
}

void SharedIsaTreeView::drawRow(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    int row_height = option.rect.height();
    if (row_height == 0)
    {
        // If the row height is zero we wouldn't paint anything anyway.
        return;
    }

    int y_coordinate_row = option.rect.y() / row_height;

    const bool   even_row               = (y_coordinate_row % 2) == 0;  // Assume first column / line number column.
    const QColor background_row_color   = QtCommon::QtUtils::ColorTheme::Get().GetCurrentThemeColors().isa_background_row_color;
    const QColor search_match_row_color = QtCommon::QtUtils::ColorTheme::Get().GetCurrentThemeColors().isa_search_match_row_color;

    // Manually paint a gray background color for every other row.

    const QRect view_index_visual_rect = visualRect(index);
    const int   view_index_y_position  = view_index_visual_rect.y();

    bool code_block_pinned = false;

    if (index.parent().isValid() && view_index_y_position == 0)
    {
        code_block_pinned = true;
    }

    // If something in this row matches the search, then give it a special background.
    if (shared_isa_widget_->DoesIndexMatchSearch(index) && !code_block_pinned)
    {
        painter->fillRect(option.rect, search_match_row_color);
    }
    else if (even_row)
    {
        painter->fillRect(option.rect, background_row_color);
    }

    // Paint the column separators.
    if (paint_column_separators_)
    {
        const QAbstractItemModel* proxy_model = index.model();

        if (proxy_model != nullptr)
        {
            int column_x_pos = -horizontalScrollBar()->value();
            for (int i = 0; i < proxy_model->columnCount(); i++)
            {
                QRect index_rect   = option.rect;
                int   column_width = header()->sectionSize(header()->logicalIndex(i));

                index_rect.setX(column_x_pos);
                index_rect.setWidth(column_width);

                painter->save();
                auto pen = painter->pen();
                pen.setColor(QtCommon::QtUtils::ColorTheme::Get().GetCurrentThemeColors().column_separator_color);
                painter->setPen(pen);
                painter->drawLine(index_rect.topRight(), index_rect.bottomRight());
                painter->restore();

                column_x_pos += column_width;
            }
        }
    }

    // Paint the rest of the rows contents on top of the background.
    ScaledTreeView::drawRow(painter, option, index);
}

void SharedIsaTreeView::keyPressEvent(QKeyEvent* event)
{
    bool event_handled = false;

    const QItemSelectionModel* selection_model = selectionModel();

    // Support Copy/Paste.
    if (selection_model != nullptr && selection_model->hasSelection())
    {
        // The keyboard modifiers are different for PC/Linux vs macOS
#ifdef Q_OS_MACOS
        if (event->key() == Qt::Key_C && (event->modifiers() & Qt::MetaModifier))
#else
        if (event->key() == Qt::Key_C && (event->modifiers() & Qt::ControlModifier))
#endif  // Q_OS_MACOS
        {
            QString clipboard_text = "";

            QModelIndexList selection = selection_model->selectedIndexes();

            std::vector<CompareIndexInfo> view_sorted_selection;

            std::map<int, int> column_max_widths;

            for (const QModelIndex& index : selection)
            {
                CompareIndexInfo compare_index_info;
                compare_index_info.source_index = index;

                SharedIsaProxyModel* proxy_model = qobject_cast<SharedIsaProxyModel*>(model());
                if (proxy_model != nullptr)
                {
                    compare_index_info.source_index = proxy_model->mapToSource(index);
                }

                const int column_index = index.column();

                compare_index_info.visual_column = header()->visualIndex(column_index);
                compare_index_info.y_pos         = visualRect(index).y();

                // Check whether line numbers should be included.
                if (column_index == SharedIsaItemModel::kLineNumber && !copy_line_numbers_)
                {
                    continue;
                }

                view_sorted_selection.push_back(compare_index_info);

                // Ignore the column widths of comments and code blocks.
                if (column_index != SharedIsaItemModel::kLineNumber && isFirstColumnSpanned(index.row(), index.parent()))
                {
                    continue;
                }

                QString text = index.data(Qt::DisplayRole).toString();

                // Indent opcodes.
                if (compare_index_info.source_index.column() == SharedIsaItemModel::kOpCode && compare_index_info.source_index.parent().isValid() &&
                    compare_index_info.source_index.data(SharedIsaItemModel::kRowTypeRole).value<SharedIsaItemModel::RowType>() ==
                        SharedIsaItemModel::RowType::kCode)
                {
                    text = QString("    " + text);
                }

                const int text_length = text.length();

                if (column_max_widths.find(compare_index_info.visual_column) != column_max_widths.end())
                {
                    const int old_max_width = column_max_widths.at(compare_index_info.visual_column);

                    if (text_length > old_max_width)
                    {
                        column_max_widths[compare_index_info.visual_column] = text_length;
                    }
                }
                else
                {
                    column_max_widths[compare_index_info.visual_column] = text_length;
                }
            }

            // The selected items returned by the selection item model are not guaranteed to be sorted; sort them so
            // they are pasted in the same order that they appear on screen.
            std::sort(view_sorted_selection.begin(), view_sorted_selection.end(), CompareModelIndices);

            int y_pos = view_sorted_selection.front().y_pos;

            for (const CompareIndexInfo& compare_index_info : view_sorted_selection)
            {
                if (compare_index_info.y_pos > y_pos)
                {
                    clipboard_text.append("\n");
                    y_pos = compare_index_info.y_pos;
                }

                QString text = compare_index_info.source_index.data(Qt::DisplayRole).toString();

                // Indent opcodes.
                if (compare_index_info.source_index.column() == SharedIsaItemModel::kOpCode && compare_index_info.source_index.parent().isValid() &&
                    compare_index_info.source_index.data(SharedIsaItemModel::kRowTypeRole).value<SharedIsaItemModel::RowType>() ==
                        SharedIsaItemModel::RowType::kCode)
                {
                    text = QString("    " + text);
                }

                QString formatted_text = QString("%1\t").arg(text, -column_max_widths[compare_index_info.visual_column]);

                clipboard_text.append(formatted_text);
                clipboard_text.append(" ");
            }

            if (!clipboard_text.isEmpty())
            {
                QClipboard* clipboard = QApplication::clipboard();
                clipboard->setText(clipboard_text);
            }

            event->accept();
            event_handled = true;
        }
    }

    SetAutoScrollObject set_auto_scroll_object(this);

    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down || event->key() == Qt::Key_PageUp || event->key() == Qt::Key_PageDown)
    {
        // Temporarily enable autoScroll so that relevant keybinds will scroll the view after moving the selected index.
        set_auto_scroll_object.EnableAutoScroll();
    }

    if (!event_handled)
    {
        ScaledTreeView::keyPressEvent(event);
    }
}

void SharedIsaTreeView::ToggleCopyLineNumbers()
{
    copy_line_numbers_ = !copy_line_numbers_;
}

void SharedIsaTreeView::RegisterIsaWidget(SharedIsaWidget* widget)
{
    shared_isa_widget_ = widget;
}

void SharedIsaTreeView::ScrollBarScrolled(int value)
{
    Q_UNUSED(value);

    QAbstractItemModel* model = this->model();
    if (nullptr != model)
    {
        const QModelIndex top_left = indexAt(QPoint(0, 0));

        const QModelIndex last_pinned_parent_index = model->index(last_pinned_row_.first, 0, QModelIndex());
        const QModelIndex last_pinned_index        = model->index(last_pinned_row_.second, 0, last_pinned_parent_index);

        if (last_pinned_index.isValid() && last_pinned_index.model() == model)
        {
            setFirstColumnSpanned(last_pinned_index.row(), last_pinned_index.parent(), false);
        }
        if (!isFirstColumnSpanned(top_left.row(), top_left.parent()))
        {
            setFirstColumnSpanned(top_left.row(), top_left.parent(), true);
            last_pinned_row_ = std::pair<int, int>(top_left.parent().row(), top_left.row());
        }
        else
        {
            ClearLastPinnedndex();
        }
    }
    // Notify the viewport to refresh.
    viewport()->update();
}
