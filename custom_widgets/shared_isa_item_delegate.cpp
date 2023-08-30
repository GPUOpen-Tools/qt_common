//=============================================================================
// Copyright (c) 2022-2023 Advanced Micro Devices, Inc. All rights reserved.
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

#include "shared_isa_item_model.h"
#include "shared_isa_tree_view.h"
#include "utils/shared_isa_dictionary.h"

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
            // Operand that is the target of a branch instruction.
            color = QtCommon::QtUtils::kIsaColorDarkMagenta;
        }
        else if (!QtCommon::QtUtils::IsaColorCodingDictionaryInstance::GetInstance().ShouldHighlight(token.token_text, color))
        {
            color = QColor("black").lighter(150);
        }

        pen.setColor(color);
        painter->setPen(pen);
    }

    painter->drawText(token_rectangle, Qt::TextSingleLine, token.token_text.c_str());
}

/// @brief Paint a black comma.
///
/// @param [in] comma_rectangle      The rectangle to paint to.
/// @param [in] painter              The painter.
/// @param [in] color_coding_enabled True to color the comma black, false to color it grey.
static void PaintCommaText(const QRect& comma_rectangle, QPainter* painter, bool color_coding_enabled)
{
    if (color_coding_enabled)
    {
        QPen   pen   = painter->pen();
        QColor color = QColor("black").lighter(150);
        pen.setColor(color);
        painter->setPen(pen);
    }

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

    QModelIndex            source_index;
    QSortFilterProxyModel* proxy = qobject_cast<QSortFilterProxyModel*>(model);

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
    case QEvent::MouseButtonRelease: {
        const QMouseEvent* mouse_event             = static_cast<QMouseEvent*>(event);
        const int          offset                  = view_->header()->sectionPosition(index.column());  // Use proxy on purpose because it wants logical index.
        const int          view_x_position         = mouse_event->pos().x();
        const int          local_x_position        = view_x_position - offset;
        int                token_under_mouse_index = -1;

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
                    view_->ShowBranchInstructionsMenu(branch_label_indices, mouse_event->globalPos());
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
    case QEvent::MouseMove: {
        mouse_over_code_block_index_  = -1;
        mouse_over_instruction_index_ = -1;
        mouse_over_token_index_       = -1;

        const QMouseEvent* mouse_event      = static_cast<QMouseEvent*>(event);
        const int          offset           = view_->header()->sectionPosition(index.column());  // Use proxy on purpose because it wants logical index.
        const int          view_x_position  = mouse_event->pos().x();
        const int          local_x_position = view_x_position - offset;

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
    default: {
        break;
    }
    }

    return false;
}

void SharedIsaItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& model_index) const
{
    const QSortFilterProxyModel* proxy_model = qobject_cast<const QSortFilterProxyModel*>(model_index.model());
    QModelIndex                  source_model_index;

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

    int  proxy_index_y_position           = -1;
    bool pinned_code_block_op_code_column = false;

    // If the top row should have a code block pinned to the top, return early unless it is the op code column.
    if (CodeBlockLabelPinnedToTop(source_model_index, model_index, proxy_index_y_position, pinned_code_block_op_code_column))
    {
        return;
    }

    QStyleOptionViewItem initialized_option = option;
    initStyleOption(&initialized_option, source_model_index);

    painter->save();

    painter->setFont(initialized_option.font);

    auto pen = painter->pen();
    pen.setColor(source_model_index.data(Qt::ForegroundRole).value<QColor>());
    painter->setPen(pen);

    if (source_model_index.column() == SharedIsaItemModel::kLineNumber)
    {
        // Right align the line numbers but with padding.

        painter->drawText(initialized_option.rect,
                          initialized_option.displayAlignment,
                          source_model_index.data(Qt::DisplayRole).toString() + SharedIsaItemModel::kColumnPadding);
    }
    else if (source_model_index.column() == SharedIsaItemModel::kOpCode)
    {
        // Bold op codes.
        auto font = painter->font();
        font.setBold(true);
        painter->setFont(font);

        if (!source_model_index.parent().isValid())
        {
            // Parent code block label.

            QRect code_block_label_rectangle = initialized_option.rect;

            // Allow the code block label/comment to be painted across multiple columns.
            code_block_label_rectangle.setWidth(view_->width() - code_block_label_rectangle.x());

            painter->drawText(code_block_label_rectangle, initialized_option.displayAlignment, source_model_index.data(Qt::DisplayRole).toString());

            const bool is_label_branch_target = source_model_index.data(SharedIsaItemModel::kLabelBranchRole).toBool();

            if (is_label_branch_target)
            {
                const std::vector<SharedIsaItemModel::Token> tokens =
                    qvariant_cast<std::vector<SharedIsaItemModel::Token>>(source_model_index.data(Qt::UserRole));

                if (!tokens.empty())
                {
                    const auto& label_token = tokens.front();
                    QPoint      label_underline_start(code_block_label_rectangle.x() + label_token.x_position_start, code_block_label_rectangle.bottom());
                    QPoint      label_underline_end(code_block_label_rectangle.x() + label_token.x_position_end, code_block_label_rectangle.bottom());

                    painter->drawLine(label_underline_start, label_underline_end);
                }
            }
        }
        else
        {
            // Child instruction.

            if (proxy_index_y_position == 0)
            {
                // If a code block should be pinned to the top, paint instruction's parent's basic block name even after scrolling beyond the basic block name.

                SharedIsaItemDelegate::paint(painter, initialized_option, source_model_index.parent().siblingAtColumn(SharedIsaItemModel::kOpCode));
            }
            else
            {
                const std::vector<SharedIsaItemModel::Token> tokens =
                    qvariant_cast<std::vector<SharedIsaItemModel::Token>>(source_model_index.data(Qt::UserRole));

                if (!tokens.empty())
                {
                    const auto& op_code_token = tokens.front();  // Assume 1 token for op code column.

                    QRect op_code_rectangle = initialized_option.rect;

                    if (op_code_token.type != SharedIsaItemModel::TokenType::kCommentType)
                    {
                        op_code_rectangle.setX(op_code_rectangle.x() +
                                               initialized_option.fontMetrics.horizontalAdvance(SharedIsaItemModel::kOpCodeColumnIndent));
                    }
                    else
                    {
                        // Allow the instruction comment to be painted across multiple columns.
                        op_code_rectangle.setWidth(view_->width() - op_code_rectangle.x());
                    }

                    PaintTokenHighlight(op_code_token,
                                        op_code_rectangle,
                                        painter,
                                        initialized_option.fontMetrics,
                                        source_model_index.parent().row(),
                                        source_model_index.row(),
                                        0);  // Assume 0 index for op code.

                    const bool color_coding_enabled = source_model_index.data(SharedIsaItemModel::kLineEnabledRole).toBool();

                    PaintTokenText(op_code_token, op_code_rectangle, painter, color_coding_enabled);
                }
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

                int        token_index          = 0;
                const bool color_coding_enabled = source_model_index.data(SharedIsaItemModel::kLineEnabledRole).toBool();
                QRect      token_rectangle      = option.rect;
                std::vector<std::vector<SharedIsaItemModel::Token>> tokens =
                    qvariant_cast<std::vector<std::vector<SharedIsaItemModel::Token>>>(source_model_index.data(Qt::UserRole));

                // Break the operands down into their individual tokens, and paint them token by token.
                for (size_t i = 0; i < tokens.size(); i++)
                {
                    const auto& operand_tokens = tokens.at(i);

                    for (size_t j = 0; j < operand_tokens.size(); j++)
                    {
                        const auto& token = operand_tokens.at(j);

                        const QString token_text(token.token_text.c_str());

                        if (token.is_selectable)
                        {
                            PaintTokenHighlight(token,
                                                token_rectangle,
                                                painter,
                                                initialized_option.fontMetrics,
                                                source_model_index.parent().row(),
                                                source_model_index.row(),
                                                static_cast<int>(token_index));
                        }

                        PaintTokenText(token, token_rectangle, painter, color_coding_enabled);

                        if (token.type == SharedIsaItemModel::TokenType::kBranchLabelType)
                        {
                            QPoint label_underline_start(token_rectangle.x() + token.x_position_start, token_rectangle.bottom());
                            QPoint label_underline_end(token_rectangle.x() + token.x_position_end, token_rectangle.bottom());

                            // Re-use color and draw a line underneath the target of the branch instruction.
                            painter->drawLine(label_underline_start, label_underline_end);
                        }

                        token_rectangle.adjust(initialized_option.fontMetrics.horizontalAdvance(token_text), 0, 0, 0);

                        // Add a space if it is not the last token in the operand.
                        if (j < operand_tokens.size() - 1)
                        {
                            token_rectangle.adjust(initialized_option.fontMetrics.horizontalAdvance(QString(" ")), 0, 0, 0);
                        }

                        token_index++;
                    }

                    // Add a comma if it is not the last operand.
                    if (i < tokens.size() - 1)
                    {
                        PaintCommaText(token_rectangle, painter, color_coding_enabled);

                        token_rectangle.adjust(initialized_option.fontMetrics.horizontalAdvance(QString(SharedIsaItemModel::kOperandDelimiter)), 0, 0, 0);
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

    return source_model->ColumnSizeHint(source_model_index.column());
}

bool SharedIsaItemDelegate::CodeBlockLabelPinnedToTop(const QModelIndex& source_model_index,
                                                      const QModelIndex& proxy_model_index,
                                                      int&               proxy_index_y_position,
                                                      bool&              pinned_code_block_op_code_column) const
{
    const QRect proxy_index_visual_rect = view_->visualRect(proxy_model_index);
    proxy_index_y_position              = proxy_index_visual_rect.y();

    if (source_model_index.parent().isValid() && proxy_index_y_position == 0)
    {
        if (source_model_index.column() != SharedIsaItemModel::kOpCode)
        {
            return true;
        }
        else
        {
            // Set the flag telling the paint function that the code block label will be drawn at this index.
            pinned_code_block_op_code_column = true;
        }
    }

    return false;
}

void SharedIsaItemDelegate::PaintColumnSeparator(QPainter* painter, const QRect& rectangle) const
{
    static const QColor column_separator_color = QColor(230, 230, 230);

    painter->save();

    auto pen = painter->pen();
    pen.setColor(column_separator_color);
    painter->setPen(pen);
    painter->drawLine(rectangle.topRight(), rectangle.bottomRight());

    painter->restore();
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

        painter->fillRect(highlighted_operand_rectangle, QtCommon::QtUtils::kIsaColorLightPink);
    }
    else if (mouse_over_isa_token_.token_text == token.token_text && code_block_index == mouse_over_code_block_index_ &&
             instruction_index == mouse_over_instruction_index_ && mouse_over_token_index_ == token_index)
    {
        // This token is underneath the mouse so highlight it.

        QRect highlighted_token_rectangle = isa_token_rectangle;
        highlighted_token_rectangle.setWidth(font_metrics.horizontalAdvance(token.token_text.c_str()));

        painter->fillRect(highlighted_token_rectangle, QtCommon::QtUtils::kIsaColorLightLightPink);
    }
}
