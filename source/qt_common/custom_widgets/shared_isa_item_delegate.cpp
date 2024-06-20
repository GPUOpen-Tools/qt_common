//=============================================================================
// Copyright (c) 2022-2024 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Implementation for a shared isa item delegate.
//=============================================================================

#include "shared_isa_item_delegate.h"

#include <QColor>
#include <QEvent>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QSortFilterProxyModel>

#include "qt_common/utils/common_definitions.h"
#include "qt_common/utils/qt_util.h"
#include "qt_common/utils/shared_isa_dictionary.h"
#include "shared_isa_item_model.h"
#include "shared_isa_tree_view.h"

/// @brief Paint a token's text using a color based on its type or syntax.
///
/// @param [in] token                The token to paint.
/// @param [in] token_rectangle      The rectangle to paint to.
/// @param [in] painter              The painter.
/// @param [in] color_coding_enabled True to apply a color coding to the token, false otherwise.
static void PaintTokenText(const SharedIsaItemModel::Token& token, const QRect& token_rectangle, QPainter* painter, const bool color_coding_enabled)
{
    if (color_coding_enabled)
    {
        QColor color;
        auto   pen = painter->pen();

        if (token.type == SharedIsaItemModel::TokenType::kBranchLabelType)
        {
            QColor operand_color;
            if (QtCommon::QtUtils::ColorTheme::Get().GetColorTheme() == kColorThemeTypeLight)
            {
                operand_color = QtCommon::QtUtils::kIsaLightThemeColorDarkMagenta;
            }
            else
            {
                operand_color = QtCommon::QtUtils::kIsaDarkThemeColorDarkMagenta;
            }

            // Operand that is the target of a branch instruction.
            color = operand_color;
        }
        else if (!QtCommon::QtUtils::IsaColorCodingDictionaryInstance::GetInstance().ShouldHighlight(token.token_text, color))
        {
            color = pen.color();
        }

        pen.setColor(color);
        painter->setPen(pen);
    }

    painter->drawText(token_rectangle, Qt::TextSingleLine, token.token_text.c_str());
}

/// @brief Paint a comma.
///
/// @param [in] comma_rectangle      The rectangle to paint to.
/// @param [in] painter              The painter.
static void PaintCommaText(const QRect& comma_rectangle, QPainter* painter)
{
    painter->drawText(comma_rectangle, ",");
}

SharedIsaItemDelegate::SharedIsaItemDelegate(SharedIsaTreeView* view, QObject* parent)
    : QStyledItemDelegate(parent)
    , view_(view)
    , mouse_over_code_block_index_(-1)
    , mouse_over_instruction_index_(-1)
    , mouse_over_token_index_(-1)
{
}

SharedIsaItemDelegate::~SharedIsaItemDelegate()
{
}

bool SharedIsaItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    Q_UNUSED(option);

    // Normal bounds checking.
    if (!index.isValid())
    {
        return true;
    }

    QModelIndex                  source_index;
    const QSortFilterProxyModel* proxy = qobject_cast<QSortFilterProxyModel*>(model);

    if (proxy != nullptr)
    {
        source_index = proxy->mapToSource(index);
    }
    else
    {
        source_index = index;
    }

    switch (event->type())
    {
    case QEvent::MouseButtonRelease:
    {
        const QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
        const qreal        offset      = view_->header()->sectionPosition(index.column());  // Use proxy on purpose because it wants logical index.

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        const qreal view_x_position = mouse_event->pos().x();
#else
        const qreal view_x_position = mouse_event->position().x();
#endif

        qreal local_x_position        = view_x_position - offset;
        int   token_under_mouse_index = -1;

        AdjustXPositionForSpannedColumns(index, proxy, source_index, local_x_position);

        SetSelectableTokenUnderMouse(source_index, local_x_position, selected_isa_token_, token_under_mouse_index);

        const bool label_clicked = SetBranchLabelTokenUnderMouse(source_index, local_x_position);

        if (label_clicked)
        {
            QVector<QModelIndex> branch_label_indices = qvariant_cast<QVector<QModelIndex>>(source_index.data(SharedIsaItemModel::kBranchIndexRole));

            if (source_index.column() == SharedIsaItemModel::kOpCode)
            {
                if (branch_label_indices.size() > 1)
                {
                    // Label is referenced by more than 1 branch instruction so show a popup menu with all of the branch instructions.
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                    view_->ShowBranchInstructionsMenu(branch_label_indices, mouse_event->globalPos());
#else
                    view_->ShowBranchInstructionsMenu(branch_label_indices, mouse_event->globalPosition().toPoint());
#endif
                }
                else
                {
                    QModelIndex source_branch_label_index = branch_label_indices.front();

                    // Label is only referenced by 1 branch instruction to just scroll to it right away.
                    view_->ScrollToBranchLabelIndex(source_branch_label_index);
                }

                // Stop processing the mouse release and scroll to the 1st corresponding label instead.
                return true;
            }
            else if (source_index.column() == SharedIsaItemModel::kOperands)
            {
                QModelIndex source_branch_label_index = branch_label_indices.front();

                // Stop processing the mouse release and scroll to the 1st corresponding label instead.
                view_->ScrollToBranchLabelIndex(source_branch_label_index);
                return true;
            }
        }

        // Tell any attached views to refresh everything.
        model->dataChanged(QModelIndex(), QModelIndex());

        break;
    }
    case QEvent::MouseMove:
    {
        mouse_over_code_block_index_  = -1;
        mouse_over_instruction_index_ = -1;
        mouse_over_token_index_       = -1;

        const QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
        const qreal        offset      = view_->header()->sectionPosition(index.column());  // Use proxy on purpose because it wants logical index.

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        const qreal view_x_position = mouse_event->pos().x();
#else

        const qreal view_x_position = mouse_event->position().x();
#endif

        qreal local_x_position = view_x_position - offset;

        AdjustXPositionForSpannedColumns(index, proxy, source_index, local_x_position);

        const bool mouse_over_isa_token = SetSelectableTokenUnderMouse(source_index, local_x_position, mouse_over_isa_token_, mouse_over_token_index_);

        if (mouse_over_isa_token)
        {
            mouse_over_code_block_index_  = source_index.parent().row();
            mouse_over_instruction_index_ = source_index.row();
        }

        SetBranchLabelTokenUnderMouse(source_index, local_x_position);

        // Tell any attached views to refresh everything.
        model->dataChanged(QModelIndex(), QModelIndex());

        break;
    }
    default:
    {
        break;
    }
    }

    return false;
}

void SharedIsaItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& model_index) const
{
    const SharedIsaProxyModel* proxy_model = qobject_cast<const SharedIsaProxyModel*>(model_index.model());
    QModelIndex                source_model_index;

    if (proxy_model == nullptr)
    {
        // No proxy being used.
        source_model_index = model_index;
    }
    else
    {
        // Proxy being used.
        source_model_index = proxy_model->mapToSource(model_index);
    }

    int  proxy_index_y_position          = -1;
    bool pinned_label_line_number_column = false;

    QStyleOptionViewItem initialized_option = option;
    initStyleOption(&initialized_option, source_model_index);

    painter->save();

    painter->setFont(initialized_option.font);

    if (CodeBlockLabelPinnedToTop(source_model_index, model_index, proxy_index_y_position, pinned_label_line_number_column))
    {
        // This index is part of a child row and is at the top of its tree's viewport.
        // Instead of painting the child row normally, paint its parent label, as if it is pinned to the top of the viewport.

        if (pinned_label_line_number_column)
        {
            // This index also belongs to the line number column.
            // Paint the entire pinned parent label row from within this column and have it span all columns.

            const QModelIndex parent_op_code_source_index = source_model_index.parent().siblingAtColumn(SharedIsaItemModel::kOpCode);
            QModelIndex       parent_op_code_proxy_index  = parent_op_code_source_index;

            if (proxy_model != nullptr)
            {
                parent_op_code_proxy_index = proxy_model->mapFromSource(parent_op_code_source_index);
            }

            PaintSpanned(painter, initialized_option, parent_op_code_source_index, parent_op_code_proxy_index);
        }
        painter->restore();
        return;
    }

    QVariant color_data = source_model_index.data(Qt::ForegroundRole);
    if (color_data != QVariant())
    {
        auto pen = painter->pen();
        pen.setColor(color_data.value<QColor>());
        painter->setPen(pen);
    }

    if (source_model_index.column() == SharedIsaItemModel::kLineNumber)
    {
        QString line_num_text = source_model_index.data(Qt::DisplayRole).toString() + SharedIsaItemModel::kColumnPadding;

        QRect line_number_rect = initialized_option.rect;

        int column_width        = view_->header()->sectionSize(view_->header()->logicalIndex(0));
        int text_width          = initialized_option.fontMetrics.horizontalAdvance(line_num_text);
        int scroll_bar_position = view_->horizontalScrollBar()->value();
        int line_number_x_pos   = column_width - text_width - scroll_bar_position;

        line_number_rect.setX(line_number_x_pos);
        line_number_rect.setWidth(initialized_option.fontMetrics.horizontalAdvance(line_num_text));

        bool                      paint_line_numbers = true;
        const SharedIsaItemModel* source_model       = qobject_cast<const SharedIsaItemModel*>(source_model_index.model());
        if (source_model != nullptr)
        {
            paint_line_numbers = source_model->LineNumbersVisible();
        }

        if (!pinned_label_line_number_column && paint_line_numbers)
        {
            painter->drawText(line_number_rect, Qt::Alignment(Qt::AlignLeft | Qt::AlignTop), line_num_text);
        }

        QModelIndex op_code_index = source_model_index.siblingAtColumn(SharedIsaItemModel::kOpCode);

        QModelIndex proxy_index = op_code_index;
        if (proxy_model != nullptr)
        {
            proxy_index = proxy_model->mapFromSource(op_code_index);
        }

        if (!source_model_index.parent().isValid())
        {
            // Paint all parent rows, instructions or comments, to span across columns.
            PaintSpanned(painter, initialized_option, op_code_index, proxy_index);
        }
        else
        {
            const auto row_type = qvariant_cast<SharedIsaItemModel::RowType>(op_code_index.data(SharedIsaItemModel::kRowTypeRole));

            if (row_type == SharedIsaItemModel::RowType::kComment)
            {
                // Paint child rows, comments only, to span across columns.
                PaintSpanned(painter, initialized_option, op_code_index, proxy_index);
            }
        }
    }
    else if (source_model_index.column() == SharedIsaItemModel::kOpCode)
    {
        // Bold op codes.
        auto font = painter->font();
        font.setBold(true);
        painter->setFont(font);

        if (source_model_index.parent().isValid() && (proxy_index_y_position != 0))
        {
            // Child row that is not at the top of the tree's viewport.

            const auto row_type = qvariant_cast<SharedIsaItemModel::RowType>(source_model_index.data(SharedIsaItemModel::kRowTypeRole));

            if (row_type != SharedIsaItemModel::RowType::kComment)
            {
                QRect op_code_rectangle = initialized_option.rect;
                op_code_rectangle.setX(op_code_rectangle.x() + initialized_option.fontMetrics.horizontalAdvance(SharedIsaItemModel::kOpCodeColumnIndent));

                const std::vector<SharedIsaItemModel::Token> op_code_token =
                    qvariant_cast<std::vector<SharedIsaItemModel::Token>>(source_model_index.data(Qt::UserRole));

                PaintText(painter, initialized_option, source_model_index, op_code_rectangle, op_code_token, 0, false);
            }
        }
    }
    else if (source_model_index.column() == SharedIsaItemModel::kOperands)
    {
        auto font = painter->font();
        font.setBold(true);
        painter->setFont(font);

        if (!source_model_index.parent().isValid())
        {
            // Parent code block label, show nothing in operand column.
        }
        else
        {
            // Child instruction.

            if (proxy_index_y_position == 0)
            {
                // Because this is the 0th row on screen, we are showing the parent label in the op code column, instead of this instruction's op code.
                // So, we don't want to show anything else in this row, so do nothing for the operand column.
            }
            else
            {
                // Paint an instruction's operands.
                std::vector<std::vector<SharedIsaItemModel::Token>> tokens =
                    qvariant_cast<std::vector<std::vector<SharedIsaItemModel::Token>>>(source_model_index.data(Qt::UserRole));

                std::pair<int, QRect> token_info = std::pair<int, QRect>(0, option.rect);

                // Break the operands down into their individual tokens, and paint them token by token.
                for (size_t i = 0; i < tokens.size(); i++)
                {
                    const auto& operand_tokens = tokens.at(i);

                    token_info = PaintText(painter, initialized_option, source_model_index, token_info.second, operand_tokens, token_info.first, false);

                    // Add a comma if it is not the last operand.
                    if (i < tokens.size() - 1)
                    {
                        PaintCommaText(token_info.second, painter);

                        token_info.second.adjust(initialized_option.fontMetrics.horizontalAdvance(QString(SharedIsaItemModel::kOperandDelimiter)), 0, 0, 0);
                    }
                }
            }
        }
    }
    else if (source_model_index.column() == SharedIsaItemModel::kPcAddress || source_model_index.column() == SharedIsaItemModel::kBinaryRepresentation)
    {
        painter->drawText(initialized_option.rect, initialized_option.displayAlignment, source_model_index.data(Qt::DisplayRole).toString());
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, model_index);
    }

    painter->restore();
}

QSize SharedIsaItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize size_hint(0, 0);

    const QAbstractProxyModel* proxy_model  = qobject_cast<const QAbstractProxyModel*>(index.model());
    const SharedIsaItemModel*  source_model = nullptr;

    if (proxy_model == nullptr)
    {
        source_model = qobject_cast<const SharedIsaItemModel*>(index.model());
    }
    else
    {
        source_model = qobject_cast<const SharedIsaItemModel*>(proxy_model->sourceModel());
    }

    if (source_model == nullptr)
    {
        return size_hint;
    }

    QModelIndex source_model_index;

    if (proxy_model == nullptr)
    {
        // No proxy being used.
        source_model_index = index;
    }
    else
    {
        // Proxy being used.
        source_model_index = proxy_model->mapToSource(index);
    }

    if (source_model_index.column() >= SharedIsaItemModel::kColumnCount)
    {
        return QStyledItemDelegate::sizeHint(option, index);
    }

    return source_model->ColumnSizeHint(source_model_index.column(), view_);
}

bool SharedIsaItemDelegate::CodeBlockLabelPinnedToTop(const QModelIndex& source_model_index,
                                                      const QModelIndex& proxy_model_index,
                                                      int&               proxy_index_y_position,
                                                      bool&              pinned_label_line_number_column) const
{
    const QRect proxy_index_visual_rect = view_->visualRect(proxy_model_index);
    proxy_index_y_position              = proxy_index_visual_rect.y();

    if (source_model_index.parent().isValid() && proxy_index_y_position == 0)
    {
        if (source_model_index.column() == SharedIsaItemModel::kLineNumber)
        {
            // Set the flag telling the paint function that the parent label will be drawn at this index instead.
            pinned_label_line_number_column = true;
        }

        return true;
    }

    return false;
}

void SharedIsaItemDelegate::AdjustXPositionForSpannedColumns(const QModelIndex&           index,
                                                             const QSortFilterProxyModel* proxy,
                                                             QModelIndex&                 source_index,
                                                             qreal&                       local_x_position)
{
    if (view_->isFirstColumnSpanned(index.row(), index.parent()))
    {
        int opcode_index = SharedIsaItemModel::kOpCode;

        if (proxy != nullptr)
        {
            opcode_index = proxy->mapFromSource(source_index.siblingAtColumn(SharedIsaItemModel::kOpCode)).column();
        }

        if (opcode_index != -1 && local_x_position > view_->header()->sectionPosition(opcode_index))
        {
            int next_index = view_->header()->logicalIndex(view_->header()->visualIndex(opcode_index) + 1);

            if (next_index == -1 || local_x_position < view_->header()->sectionPosition(next_index))
            {
                source_index = source_index.siblingAtColumn(SharedIsaItemModel::kOpCode);
                local_x_position -= view_->header()->sectionPosition(opcode_index);
            }
        }
    }
}

bool SharedIsaItemDelegate::SetSelectableTokenUnderMouse(const QModelIndex&         source_index,
                                                         const int                  local_x_position,
                                                         SharedIsaItemModel::Token& isa_token_under_mouse,
                                                         int&                       isa_token_under_mouse_index)
{
    isa_token_under_mouse.Clear();
    isa_token_under_mouse_index = -1;

    if (source_index.column() != SharedIsaItemModel::Columns::kOpCode && source_index.column() != SharedIsaItemModel::Columns::kOperands)
    {
        return false;
    }

    std::vector<SharedIsaItemModel::Token> tokens;

    if (source_index.column() == SharedIsaItemModel::Columns::kOpCode)
    {
        tokens = qvariant_cast<std::vector<SharedIsaItemModel::Token>>(source_index.data(Qt::UserRole));
    }
    else
    {
        const std::vector<std::vector<SharedIsaItemModel::Token>> tokens_per_operands =
            qvariant_cast<std::vector<std::vector<SharedIsaItemModel::Token>>>(source_index.data(Qt::UserRole));

        for (const auto& tokens_per_operand : tokens_per_operands)
        {
            tokens.insert(tokens.end(), tokens_per_operand.begin(), tokens_per_operand.end());
        }
    }

    for (int i = 0; i < static_cast<int>(tokens.size()); i++)
    {
        const auto& isa_token = tokens.at(i);

        if (isa_token.is_selectable && local_x_position >= isa_token.x_position_start && local_x_position <= isa_token.x_position_end)
        {
            isa_token_under_mouse_index = i;
            isa_token_under_mouse       = isa_token;
            return true;
        }
    }

    return false;
}

bool SharedIsaItemDelegate::SetBranchLabelTokenUnderMouse(const QModelIndex& source_index, const int local_x_position)
{
    bool hover_over_label = false;

    if (source_index.column() == SharedIsaItemModel::kOpCode)
    {
        const bool is_mouse_over_branch_label = source_index.data(SharedIsaItemModel::kLabelBranchRole).toBool();

        if (is_mouse_over_branch_label)
        {
            // Label is referenced by branch instructions.

            std::vector<SharedIsaItemModel::Token> tokens = qvariant_cast<std::vector<SharedIsaItemModel::Token>>(source_index.data(Qt::UserRole));

            SharedIsaItemModel::Token token;

            if (!tokens.empty())
            {
                token = tokens.front();
            }

            if (token.type == SharedIsaItemModel::TokenType::kLabelType && local_x_position >= token.x_position_start &&
                local_x_position <= token.x_position_end)
            {
                hover_over_label = true;

                view_->setCursor(Qt::PointingHandCursor);
            }
        }
    }
    else if (source_index.column() == SharedIsaItemModel::kOperands)
    {
        std::vector<std::vector<SharedIsaItemModel::Token>> tokens =
            qvariant_cast<std::vector<std::vector<SharedIsaItemModel::Token>>>(source_index.data(Qt::UserRole));

        SharedIsaItemModel::Token token;

        if (!tokens.empty() && !tokens.front().empty())
        {
            token = tokens.front().front();
        }

        if (token.type == SharedIsaItemModel::TokenType::kBranchLabelType && local_x_position >= token.x_position_start &&
            local_x_position <= token.x_position_end)
        {
            hover_over_label = true;

            view_->setCursor(Qt::PointingHandCursor);
        }
    }

    if (!hover_over_label)
    {
        view_->setCursor(Qt::ArrowCursor);
    }

    return hover_over_label;
}

void SharedIsaItemDelegate::PaintTokenHighlight(const SharedIsaItemModel::Token& token,
                                                const QRect&                     isa_token_rectangle,
                                                QPainter*                        painter,
                                                const QFontMetrics&              font_metrics,
                                                int                              code_block_index,
                                                int                              instruction_index,
                                                int                              token_index) const
{
    bool is_token_selected = false;

    if (token.type == SharedIsaItemModel::TokenType::kScalarRegisterType || token.type == SharedIsaItemModel::TokenType::kVectorRegisterType)
    {
        // Check if register numbers match.

        if (((token.type == SharedIsaItemModel::TokenType::kScalarRegisterType &&
              selected_isa_token_.type == SharedIsaItemModel::TokenType::kScalarRegisterType) ||
             (token.type == SharedIsaItemModel::TokenType::kVectorRegisterType &&
              selected_isa_token_.type == SharedIsaItemModel::TokenType::kVectorRegisterType)))
        {
            // Comparing scalar to scalar or vector to vector.

            if (selected_isa_token_.start_register_index != -1 && token.start_register_index != -1)
            {
                // End can be -1 but start should never be -1.

                if (selected_isa_token_.end_register_index == -1 && token.end_register_index != -1)
                {
                    // The selected/clicked token is a single register but the token we are checking is a range.

                    if (selected_isa_token_.start_register_index >= token.start_register_index &&
                        selected_isa_token_.start_register_index <= token.end_register_index)
                    {
                        is_token_selected = true;
                    }
                }
                else if (token.end_register_index == -1 && selected_isa_token_.end_register_index != -1)
                {
                    // The token we are checking is a single register but the selected/clicked is a range.

                    if (token.start_register_index >= selected_isa_token_.start_register_index &&
                        token.start_register_index <= selected_isa_token_.end_register_index)
                    {
                        is_token_selected = true;
                    }
                }
                else if (token.end_register_index == -1 && selected_isa_token_.end_register_index == -1)
                {
                    // Both tokens are a single register.

                    if (token.start_register_index == selected_isa_token_.start_register_index)
                    {
                        is_token_selected = true;
                    }
                }
                else if ((token.start_register_index >= selected_isa_token_.start_register_index &&
                          token.start_register_index <= selected_isa_token_.end_register_index) ||
                         (token.end_register_index >= selected_isa_token_.start_register_index &&
                          token.end_register_index <= selected_isa_token_.end_register_index))

                {
                    // Both tokens are ranges and the ranges overlap.

                    is_token_selected = true;
                }
            }
        }
    }
    else
    {
        // Op code or constant, string check should be sufficient.
        if (selected_isa_token_.token_text == token.token_text)
        {
            is_token_selected = true;
        }
    }

    if (is_token_selected)
    {
        // This token matches the currently selected token so highlight it.

        QRect highlighted_operand_rectangle = isa_token_rectangle;
        highlighted_operand_rectangle.setWidth(font_metrics.horizontalAdvance(token.token_text.c_str()));

        QColor higlight_color;
        if (QtCommon::QtUtils::ColorTheme::Get().GetColorTheme() == kColorThemeTypeLight)
        {
            higlight_color = QtCommon::QtUtils::kIsaLightThemeColorLightPink;
        }
        else
        {
            higlight_color = QtCommon::QtUtils::kIsaDarkThemeColorDarkDarkPurple;
        }

        painter->fillRect(highlighted_operand_rectangle, higlight_color);
    }
    else if (mouse_over_isa_token_.token_text == token.token_text && code_block_index == mouse_over_code_block_index_ &&
             instruction_index == mouse_over_instruction_index_ && mouse_over_token_index_ == token_index)
    {
        // This token is underneath the mouse so highlight it.

        QRect highlighted_token_rectangle = isa_token_rectangle;
        highlighted_token_rectangle.setWidth(font_metrics.horizontalAdvance(token.token_text.c_str()));

        QColor higlight_color;
        if (QtCommon::QtUtils::ColorTheme::Get().GetColorTheme() == kColorThemeTypeLight)
        {
            higlight_color = QtCommon::QtUtils::kIsaLightThemeColorLightPink;
        }
        else
        {
            higlight_color = QtCommon::QtUtils::kIsaDarkThemeColorDarkDarkPurple;
        }

        painter->fillRect(highlighted_token_rectangle, higlight_color);
    }
}

void SharedIsaItemDelegate::PaintSpanned(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& source_index, QModelIndex proxy_index) const
{
    painter->save();

    auto font = source_index.data(Qt::FontRole).value<QFont>();
    font.setBold(true);
    painter->setFont(font);

    QVariant op_code_color_data = source_index.data(Qt::ForegroundRole);

    if (op_code_color_data != QVariant())
    {
        auto pen = painter->pen();
        pen.setColor(op_code_color_data.value<QColor>());
        painter->setPen(pen);
    }

    const std::vector<SharedIsaItemModel::Token> tokens = qvariant_cast<std::vector<SharedIsaItemModel::Token>>(source_index.data(Qt::UserRole));
    const auto row_type   = qvariant_cast<SharedIsaItemModel::RowType>(source_index.data(SharedIsaItemModel::UserRoles::kRowTypeRole));
    const bool is_comment = row_type == SharedIsaItemModel::RowType::kComment;

    // Determine where to start painting the spanning text.
    int index_x_pos = 0;

    if (is_comment || !proxy_index.isValid())
    {
        // Paint spanning text for a comment or for a label while the op code column is not visible.
        // Start painting right after the line #.
        index_x_pos = view_->header()->sectionPosition(view_->header()->logicalIndex(1));
    }
    else
    {
        // Paint spanning text for a label while the op code column is visible.
        // Start painting at the op code column.
        index_x_pos = view_->header()->sectionPosition(proxy_index.column());
    }

    index_x_pos -= view_->horizontalScrollBar()->value();

    QRect text_rectangle = option.rect;
    text_rectangle.setX(index_x_pos);
    text_rectangle.setWidth(view_->width() - text_rectangle.x());

    PaintText(painter, option, source_index, text_rectangle, tokens, 0, is_comment);

    painter->restore();
}

std::pair<int, QRect> SharedIsaItemDelegate::PaintText(QPainter*                              painter,
                                                       const QStyleOptionViewItem&            option,
                                                       const QModelIndex&                     source_index,
                                                       QRect                                  token_rectangle,
                                                       std::vector<SharedIsaItemModel::Token> tokens,
                                                       int                                    token_index,
                                                       bool                                   is_comment) const
{
    if (is_comment)
    {
        painter->drawText(token_rectangle, Qt::Alignment(Qt::AlignLeft | Qt::AlignTop), source_index.data(Qt::DisplayRole).toString());
    }
    else if (!source_index.parent().isValid())
    {
        if (!tokens.empty())
        {
            const bool color_coding_enabled = source_index.data(SharedIsaItemModel::kLineEnabledRole).toBool();
            painter->save();
            PaintTokenText(tokens.front(), token_rectangle, painter, color_coding_enabled);
            painter->restore();
        }
    }
    else
    {
        for (size_t i = 0; i < tokens.size(); i++)
        {
            const auto token = tokens.at(i);

            if (token.is_selectable)
            {
                PaintTokenHighlight(token,
                                    token_rectangle,
                                    painter,
                                    option.fontMetrics,
                                    source_index.parent().row(),
                                    source_index.row(),
                                    token_index);  // Assume 0 index for op code.
            }

            const bool color_coding_enabled = source_index.data(SharedIsaItemModel::kLineEnabledRole).toBool();

            painter->save();
            PaintTokenText(token, token_rectangle, painter, color_coding_enabled);
            if (token.type == SharedIsaItemModel::TokenType::kBranchLabelType)
            {
                QPoint label_underline_start(token_rectangle.x() + token.x_position_start, token_rectangle.bottom());
                QPoint label_underline_end(token_rectangle.x() + token.x_position_end, token_rectangle.bottom());

                // Re-use color and draw a line underneath the target of the branch instruction.
                painter->drawLine(label_underline_start, label_underline_end);
            }
            painter->restore();
            const QString token_text(token.token_text.c_str());
            token_rectangle.adjust(option.fontMetrics.horizontalAdvance(token_text), 0, 0, 0);

            // Add a space if it is not the last token in the operand.
            if (i < tokens.size() - 1)
            {
                token_rectangle.adjust(option.fontMetrics.horizontalAdvance(SharedIsaItemModel::kOperandTokenSpace), 0, 0, 0);
            }

            token_index++;
        }
    }

    return std::pair<int, QRect>(token_index, token_rectangle);
}
