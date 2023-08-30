//=============================================================================
// Copyright (c) 2022-2023 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Implementation for a shared isa item model.
//=============================================================================

#include "shared_isa_item_model.h"

#include "qt_common/utils/shared_isa_dictionary.h"
#include "qt_common/utils/qt_util.h"
#include "qt_common/utils/common_definitions.h"

#include <algorithm>

#include <QColor>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QStringList>

const QString SharedIsaItemModel::kColumnPadding      = " ";      // Pad columns by 1 character.
const QString SharedIsaItemModel::kOpCodeColumnIndent = "     ";  // Indent op code column by 5 characters.
const QString SharedIsaItemModel::kOperandDelimiter   = ", ";     // Separate operands by a comma and a space.

SharedIsaItemModel::SharedIsaItemModel(QObject* parent)
    : QAbstractItemModel(parent)
    , fixed_font_character_width_(0)
{
}

SharedIsaItemModel::~SharedIsaItemModel()
{
}

int SharedIsaItemModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return Columns::kColumnCount;
}

int SharedIsaItemModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0)
    {
        // Restrict row count to anything but the 0th column.
        // Child nodes should only be parented to the 0th column.
        return 0;
    }

    if (!parent.isValid())
    {
        // The number of top-level nodes is the number of code blocks.
        return static_cast<int>(code_blocks_.size());
    }

    if (!parent.parent().isValid())
    {
        // The number of rows underneath a Code Block is the number of instructions in that Code Block.
        return static_cast<int>(code_blocks_.at(parent.row()).instruction_lines.size());
    }

    // Instructions should not have any rows underneath them.
    return 0;
}

QModelIndex SharedIsaItemModel::index(int row, int column, const QModelIndex& parent) const
{
    QModelIndex tree_index;

    if (!hasIndex(row, column, parent))
    {
        return tree_index;
    }

    // Code blocks are top level nodes; create an index with no internal data.
    if (!parent.isValid())
    {
        return createIndex(row, column, nullptr);
    }

    // Individual instruction lines are child nodes; attach parent row index as internal data.
    return createIndex(row, column, (void*)&code_blocks_[parent.row()]);
}

QModelIndex SharedIsaItemModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    const CodeBlock* code_block = static_cast<CodeBlock*>(index.internalPointer());

    if (code_block != nullptr)
    {
        return createIndex(code_block->position, 0);
    }

    return QModelIndex();
}

QVariant SharedIsaItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant header_data;

    if (orientation == Qt::Vertical || role != Qt::DisplayRole || section < 0 || section >= static_cast<int>(column_names_.size()))
    {
        return header_data;
    }

    header_data = column_names_[section].c_str();

    return header_data;
}

QVariant SharedIsaItemModel::data(const QModelIndex& index, int role) const
{
    QVariant data;

    switch (role)
    {
    case Qt::FontRole: {
        data.setValue(fixed_font_);
        break;
    }
    case Qt::TextAlignmentRole: {
        int alignment_flags = 0;

        if (index.column() == kLineNumber)
        {
            alignment_flags = Qt::AlignRight | Qt::AlignTop;
        }
        else
        {
            alignment_flags = Qt::AlignLeft | Qt::AlignTop;
        }

        data.setValue(alignment_flags);
        break;
    }
    case Qt::ForegroundRole: {
        // This role is not useful for columns with multi-coloring in the same column.

        // Default to color theme's text color.
        data.setValue(QtCommon::QtUtils::ColorTheme::Get().GetCurrentThemeColors().graphics_scene_text_color);

        if (!index.parent().isValid() && index.column() == kOpCode)
        {
            // Provide a different starting color for code block comments and code block labels with matching branching instructions.

            const auto& code_block = code_blocks_.at(index.row());
            
            if (code_block.row_type == RowType::kComment)
            {
                QColor comment_color;
                if (QtCommon::QtUtils::ColorTheme::Get().GetColorTheme() == kColorThemeTypeLight)
                {
                    comment_color = QtCommon::QtUtils::kIsaLightThemeColorLightBlue;
                }
                else
                {
                    comment_color = QtCommon::QtUtils::kIsaDarkThemeColorLightBlue;
                }

                // This is a code block comment; provide light blue as its text color.
                data.setValue(comment_color);
            }
            else if (code_block.row_type == RowType::kCodeBlock && !code_block.mapped_branch_instructions.empty())
            {
                QColor label_color;
                if (QtCommon::QtUtils::ColorTheme::Get().GetColorTheme() == kColorThemeTypeLight)
                {
                    label_color = QtCommon::QtUtils::kIsaLightThemeColorDarkMagenta;
                }
                else
                {
                    label_color = QtCommon::QtUtils::kIsaDarkThemeColorDarkMagenta;
                }

                // This is a code block label that is called by a branch instruction(s); provide purple as its text color.
                data.setValue(label_color);
            }
        }

        break;
    }
    case Qt::DisplayRole: {
        const QModelIndex parent_index = index.parent();

        switch (index.column())
        {
        case kLineNumber: {
            int line_number = 0;

            if (!parent_index.isValid())
            {
                // Code block.
                line_number = code_blocks_.at(index.row()).line_number;
            }
            else
            {
                // Instruction line.
                line_number = code_blocks_.at(parent_index.row()).instruction_lines.at(index.row()).line_number;
            }

            data.setValue(line_number);
            break;
        }
        case kOpCode: {
            if (!index.parent().isValid())
            {
                // Code block.
                data.setValue(QString(code_blocks_.at(index.row()).token.token_text.c_str()));
            }
            else if ((code_blocks_.at(index.parent().row()).instruction_lines.at(index.row()).row_type == RowType::kInstruction) ||
                     (code_blocks_.at(index.parent().row()).instruction_lines.at(index.row()).row_type == RowType::kComment))
            {
                // Instruction line.
                data.setValue(QString(code_blocks_.at(index.parent().row()).instruction_lines.at(index.row()).op_code.c_str()));
            }
            break;
        }
        case kOperands: {
            if (index.parent().isValid() && (code_blocks_.at(index.parent().row()).instruction_lines.at(index.row()).row_type == RowType::kInstruction))
            {
                std::string                     operands_string = "";
                const std::vector<std::string>& operands        = code_blocks_.at(index.parent().row()).instruction_lines.at(index.row()).operands;

                if (!operands.empty())
                {
                    for (size_t i = 0; i < operands.size(); i++)
                    {
                        const std::string& arg = operands[i];
                        operands_string += arg;

                        if (i != operands.size() - 1)
                        {
                            operands_string += kOperandDelimiter.toStdString();
                        }
                    }
                }

                data.setValue(QString(operands_string.c_str()));
            }
            break;
        }
        case kPcAddress: {
            if (index.parent().isValid() && (code_blocks_.at(index.parent().row()).instruction_lines.at(index.row()).row_type == RowType::kInstruction))
            {
                // Instruction line.
                data.setValue(QString(code_blocks_.at(index.parent().row()).instruction_lines.at(index.row()).pc_address.c_str()));
            }

            break;
        }
        case kBinaryRepresentation: {
            if (index.parent().isValid() && (code_blocks_.at(index.parent().row()).instruction_lines.at(index.row()).row_type == RowType::kInstruction))
            {
                // Instruction line.
                data.setValue(QString(code_blocks_.at(index.parent().row()).instruction_lines.at(index.row()).binary_representation.c_str()));
            }

            break;
        }
        default: {
            break;
        }
        }
        break;
    }
    case Qt::UserRole: {
        // Use UserRole to store Tokens.

        if (!index.parent().isValid())
        {
            std::vector<Token> tokens;

            tokens.push_back(code_blocks_.at(index.row()).token);

            data.setValue(tokens);
        }
        else
        {
            if (index.column() == kOpCode)
            {
                const auto& instruction_line = code_blocks_.at(index.parent().row()).instruction_lines.at(index.row());

                if (instruction_line.row_type == RowType::kInstruction || instruction_line.row_type == RowType::kComment)
                {
                    std::vector<Token> tokens;

                    tokens.push_back(instruction_line.op_code_token);

                    data.setValue(tokens);
                }
            }
            else if (index.column() == kOperands)
            {
                const auto& instruction_line = code_blocks_.at(index.parent().row()).instruction_lines.at(index.row());

                if (instruction_line.row_type == RowType::kInstruction)
                {
                    data.setValue(instruction_line.operand_tokens);
                }
            }
        }

        break;
    }
    case kLabelBranchRole: {
        bool is_code_block_label_branch_target = false;

        if (!index.parent().isValid())
        {
            is_code_block_label_branch_target = !code_blocks_.at(index.row()).mapped_branch_instructions.empty();
        }

        data.setValue(is_code_block_label_branch_target);
        break;
    }
    case kBranchIndexRole: {
        QVector<QModelIndex> branch_label_indices;

        if (index.column() == kOpCode)
        {
            if (!index.parent().isValid())
            {
                for (const auto& mapped_branch_instruction : code_blocks_.at(index.row()).mapped_branch_instructions)
                {
                    const int         label_code_block_index  = mapped_branch_instruction.first;
                    const int         label_instruction_index = mapped_branch_instruction.second;
                    const QModelIndex branch_label_index      = this->index(label_instruction_index, 0, this->index(label_code_block_index, 0));
                    branch_label_indices.push_back(branch_label_index);
                }
            }
        }
        else if (index.column() == kOperands)
        {
            if (index.parent().isValid())
            {
                const auto& instruction_line = code_blocks_.at(index.parent().row()).instruction_lines.at(index.row());
                const auto& operand_tokens   = instruction_line.operand_tokens;

                if (!operand_tokens.empty() && !operand_tokens.front().empty())
                {
                    // Check the first token in the first operand.

                    const auto& token = instruction_line.operand_tokens.front().front();

                    if (token.type == TokenType::kBranchLabelType && token.start_register_index != -1)
                    {
                        const QModelIndex label_index = this->index(token.start_register_index, 0);
                        branch_label_indices.push_back(label_index);
                    }
                }
            }
        }

        data.setValue(branch_label_indices);
        break;
    }
    case kLineEnabledRole: {
        bool line_enabled = true;

        if (index.parent().isValid())
        {
            line_enabled = code_blocks_.at(index.parent().row()).instruction_lines.at(index.row()).enabled;
        }

        data.setValue(line_enabled);
        break;
    }
    default: {
        break;
    }
    }

    return data;
}

void SharedIsaItemModel::CacheSizeHints()
{
    column_widths_.fill(0);
    line_number_corresponding_indices_.clear();

    uint32_t max_pc_address_length            = 0;
    uint32_t max_op_code_length               = 0;
    uint32_t max_operand_length               = 0;
    uint32_t max_binary_representation_length = 0;

    if (code_blocks_.empty())
    {
        return;
    }

    int code_block_index = 0;

    for (auto& code_block : code_blocks_)
    {
        line_number_corresponding_indices_.emplace_back(-1, code_block_index);

        int instruction_index = 0;

        for (auto& instruction : code_block.instruction_lines)
        {
            // Instructions.

            line_number_corresponding_indices_.emplace_back(code_block_index, instruction_index++);

            if (instruction.row_type == RowType::kComment)
            {
                // Don't force comments to fit in the op code column.
                continue;
            }

            max_op_code_length    = std::max(max_op_code_length, static_cast<uint32_t>(instruction.op_code.length()));
            max_pc_address_length = std::max(max_pc_address_length, static_cast<uint32_t>(instruction.pc_address.length()));

            std::string operands;

            for (const auto& operand : instruction.operands)
            {
                operands += operand;
                operands += kOperandDelimiter.toStdString();
            }

            max_operand_length               = std::max(max_operand_length, static_cast<uint32_t>(operands.size()));
            max_binary_representation_length = std::max(max_binary_representation_length, static_cast<uint32_t>(instruction.binary_representation.size()));
        }

        code_block_index++;
    }

    const QFontMetrics metrics = QFontMetrics(fixed_font_);

    QString line_number_text;

    if (!code_blocks_.back().instruction_lines.empty())
    {
        line_number_text = QString::number(code_blocks_.back().instruction_lines.back().line_number) + kColumnPadding;
    }

    const QString pc_address_text(max_pc_address_length, ' ');
    const QString op_code_text = QString(max_op_code_length, ' ') + kOpCodeColumnIndent;
    const QString operand_text(max_operand_length, ' ');
    const QString binary_representation_text(max_binary_representation_length, ' ');

    const auto line_number_length           = metrics.horizontalAdvance(line_number_text);
    const auto pc_address_length            = metrics.horizontalAdvance(pc_address_text);
    const auto op_code_length               = metrics.horizontalAdvance(op_code_text);
    const auto operand_length               = metrics.horizontalAdvance(operand_text);
    const auto binary_representation_length = metrics.horizontalAdvance(binary_representation_text);

    column_widths_[kLineNumber]           = line_number_length;
    column_widths_[kPcAddress]            = pc_address_length;
    column_widths_[kOpCode]               = op_code_length;
    column_widths_[kOperands]             = operand_length;
    column_widths_[kBinaryRepresentation] = binary_representation_length;
}

QSize SharedIsaItemModel::ColumnSizeHint(int column_index) const
{
    QSize size_hint(0, 0);

    if (column_index < 0 || column_index >= kColumnCount)
    {
        return size_hint;
    }

    size_hint.setHeight(QFontMetrics(fixed_font_).height() + 2);
    size_hint.setWidth(column_widths_[column_index]);

    return size_hint;
}

void SharedIsaItemModel::SetFixedFont(const QFont& fixed_font)
{
    fixed_font_ = fixed_font;

    QFontMetrics font_metrics(fixed_font_);

    fixed_font_character_width_ = font_metrics.horizontalAdvance('T');
}

QModelIndex SharedIsaItemModel::GetLineNumberModelIndex(int line_number)
{
    if (line_number < 0 || line_number >= static_cast<int>(line_number_corresponding_indices_.size()))
    {
        return QModelIndex();
    }

    const auto& row_pair   = line_number_corresponding_indices_.at(line_number);
    const auto& parent_row = row_pair.first;
    const auto& child_row  = row_pair.second;

    const QModelIndex parent_index = index(parent_row, 0);

    return index(child_row, 0, parent_index);
}

void SharedIsaItemModel::MapBlocksToBranchInstructions()
{
    if (code_blocks_.empty())
    {
        return;
    }

    for (auto& outer_block : code_blocks_)
    {
        for (auto& inner_block : code_blocks_)
        {
            int instruction_index = 0;

            for (auto& inner_block_instruction : inner_block.instruction_lines)
            {
                for (const auto& operand_string : inner_block_instruction.operands)
                {
                    if (operand_string == outer_block.token.token_text)
                    {
                        outer_block.mapped_branch_instructions.emplace_back(std::make_pair(inner_block.position, instruction_index));

                        inner_block_instruction.operand_tokens.front().front().start_register_index = outer_block.position;
                    }
                }

                instruction_index++;
            }
        }
    }
}