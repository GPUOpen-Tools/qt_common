//=============================================================================
// Copyright (c) 2022-2023 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Shared isa tree view implementation.
//=============================================================================

#include "shared_isa_tree_view.h"

#include <algorithm>
#include <utility>

#include <QApplication>
#include <QClipboard>
#include <QFontDatabase>
#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QScrollBar>
#include <QSortFilterProxyModel>

#include "shared_isa_item_delegate.h"
#include "shared_isa_widget.h"

/// @brief Compare function that sorts model indices by their position on screen.
///
/// @param lhs The left hand side model index.
/// @param rhs The right hand side model index.
///
/// @return true if the left model index is les than the right model index, false otherwise.
static bool CompareModelIndices(const std::pair<QRect, QModelIndex>& lhs, const std::pair<QRect, QModelIndex>& rhs)
{
    const QRect& lhs_rect = lhs.first;
    const QRect& rhs_rect = rhs.first;

    if (lhs_rect.y() == rhs_rect.y())
    {
        return lhs_rect.x() < rhs_rect.x();
    }

    return lhs_rect.y() < rhs_rect.y();
}

SharedIsaTreeView::SharedIsaTreeView(QWidget* parent)
    : ScaledTreeView(parent)
    , copy_line_numbers_(true)
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

    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &SharedIsaTreeView::ScrollBarScrolled);

    // Allow contiguous selection per rows.
    setSelectionMode(QAbstractItemView::SelectionMode::ContiguousSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    // Disable alternating row colors and have this view paint them manually instead.
    // The default alternating row color behavior would otherwise paint over our attempt to let labels and comments span columns.
    setAlternatingRowColors(false);
}

SharedIsaTreeView::~SharedIsaTreeView()
{
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
    static bool         alternate_row          = false;
    static const QColor background_row_color   = QColor(240, 240, 240);
    static const QColor search_match_row_color = QColor(192, 192, 255);

    // Manually paint a gray background color for every other row.

    const QRect view_index_visual_rect = visualRect(index);
    const int   view_index_y_position  = view_index_visual_rect.y();

    if (view_index_y_position == 0)
    {
        alternate_row = false;
    }

    // If something in this row matches the search, then give it a special background.
    if (shared_isa_widget_->DoesIndexMatchSearch(index))
    {
        painter->fillRect(option.rect, search_match_row_color);
    }
    else if (alternate_row)
    {
        painter->fillRect(option.rect, background_row_color);
    }

    alternate_row = !alternate_row;

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

            std::vector<std::pair<QRect, QModelIndex>> view_sorted_selection;

            std::map<int, int> column_max_widths;

            for (const QModelIndex& source_index : selection)
            {
                const QRect index_view_rectangle = visualRect(source_index);

                std::pair<QRect, QModelIndex> rect_index_pair(index_view_rectangle, source_index);

                const int column_index = source_index.column();

                // Check Whether line numbers should be included.
                if (column_index == SharedIsaItemModel::kLineNumber && !copy_line_numbers_)
                {
                    continue;
                }

                view_sorted_selection.push_back(rect_index_pair);

                const QString text        = source_index.data(Qt::DisplayRole).toString();
                const int     text_length = text.length();

                if (column_max_widths.find(column_index) != column_max_widths.end())
                {
                    const int old_max_width = column_max_widths.at(column_index);

                    if (text_length > old_max_width)
                    {
                        column_max_widths[column_index] = text_length;
                    }
                }
                else
                {
                    column_max_widths[column_index] = text_length;
                }
            }

            // The selected items returned by the selection item model are not guaranteed to be sorted; sort them so
            // they are pasted in the same order that they appear on screen.
            std::sort(view_sorted_selection.begin(), view_sorted_selection.end(), CompareModelIndices);

            int row = view_sorted_selection.front().first.y();

            for (const std::pair<QRect, QModelIndex>& pair : view_sorted_selection)
            {
                QModelIndex source_index = pair.second;

                if (pair.first.y() > row)
                {
                    clipboard_text.append("\n");
                    row = pair.first.y();
                }

                const QString text = source_index.data(Qt::DisplayRole).toString();

                QString formatted_text = QString("%1\t").arg(text, -column_max_widths[source_index.column()]);

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

    QVector<int> roles;
    roles.push_back(Qt::DisplayRole);

    QAbstractItemModel* model        = this->model();
    const QModelIndex   top_left     = indexAt(QPoint(0, 0));
    const QModelIndex   bottom_right = indexAt(QPoint(viewport()->width() - 1, 0));

    // Notify the model/view/controller to refresh the row at the top of the view.
    emit model->dataChanged(top_left, bottom_right, roles);
}
