//=============================================================================
// Copyright (c) 2022-2024 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Implementation for a shared isa item model.
//=============================================================================

#include "shared_isa_item_model.h"

#include <algorithm>

#include <QColor>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QStringList>

#include "qt_common/custom_widgets/shared_isa_tree_view.h"
#include "qt_common/utils/common_definitions.h"
#include "qt_common/utils/qt_util.h"
#include "qt_common/utils/shared_isa_dictionary.h"

const QString     SharedIsaItemModel::kColumnPadding             = " ";           // Pad columns by 1 character.
const QString     SharedIsaItemModel::kOpCodeColumnIndent        = "     ";       // Indent op code column by 5 characters.
const QString     SharedIsaItemModel::kOperandTokenSpace         = " ";           // Separate tokens within the same operand.
const QString     SharedIsaItemModel::kOperandDelimiter          = ", ";          // Separate operands by a comma and a space.
const std::string SharedIsaItemModel::kUnconditionalBranchString = "s_branch";    // Branch op code text.
const std::string SharedIsaItemModel::kConditionalBranchString   = "s_cbranch_";  // Conditional branch op code text.

// Avoid repetitive string conversions.
const std::string kOperandTokenSpaceStdString = SharedIsaItemModel::kOperandTokenSpace.toStdString();
const std::string kOperandDelimiterStdString  = SharedIsaItemModel::kOperandDelimiter.toStdString();

SharedIsaItemModel::SharedIsaItemModel(QObject* parent)
    : QAbstractItemModel(parent)
    , fixed_font_character_width_(0)
    , line_numbers_visible_(true)
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
        return static_cast<int>(blocks_.size());
    }

    if (!parent.parent().isValid())
    {
        // The number of rows underneath a Code Block is the number of instructions in that Code Block.
        return static_cast<int>(blocks_.at(parent.row())->instruction_lines.size());
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
    return createIndex(row, column, (void*)blocks_[parent.row()].get());
}

QModelIndex SharedIsaItemModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    const InstructionBlock* code_block = static_cast<InstructionBlock*>(index.internalPointer());

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
    case Qt::FontRole:
    {
        data.setValue(fixed_font_);
        break;
    }
    case Qt::TextAlignmentRole:
    {
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
    case Qt::ForegroundRole:
    {
        // This role is not useful for columns with multi-coloring in the same column.

        // Default to color theme's text color.
        data.setValue(QtCommon::QtUtils::ColorTheme::Get().GetCurrentThemeColors().graphics_scene_text_color);

        if (!index.parent().isValid() && index.column() == kOpCode)
        {
            // Provide a different starting color for code block comments and code block labels with matching branching instructions.

            const auto block = blocks_.at(index.row());

            if (block->row_type == RowType::kComment)
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
            else if (block->row_type == RowType::kCode)
            {
                const auto code_block = std::static_pointer_cast<InstructionBlock>(block);

                if (!code_block->mapped_branch_instructions.empty())
                {
                    // This is a code block label that is called by a branch instruction(s); provide purple as its text color.
                    QColor label_color;

                    if (QtCommon::QtUtils::ColorTheme::Get().GetColorTheme() == kColorThemeTypeLight)
                    {
                        label_color = QtCommon::QtUtils::kIsaLightThemeColorDarkMagenta;
                    }
                    else
                    {
                        label_color = QtCommon::QtUtils::kIsaDarkThemeColorDarkMagenta;
                    }

                    data.setValue(label_color);
                }
            }
        }
        else if (index.parent().isValid() && index.column() != kLineNumber)
        {
            // Provide a different starting color for child row comments

            const auto row = blocks_.at(index.parent().row())->instruction_lines.at(index.row());

            if (row->row_type == RowType::kComment)
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

                data.setValue(comment_color);
            }
        }

        break;
    }
    case Qt::DisplayRole:
    {
        const QModelIndex parent_index = index.parent();

        switch (index.column())
        {
        case kLineNumber:
        {
            int line_number = 0;

            if (!parent_index.isValid())
            {
                // Code block.
                line_number = blocks_.at(index.row())->line_number;
            }
            else
            {
                // Instruction line.
                line_number = blocks_.at(parent_index.row())->instruction_lines.at(index.row())->line_number;
            }

            data.setValue(line_number);
            break;
        }
        case kOpCode:
        {
            if (!index.parent().isValid())
            {
                const auto block = blocks_.at(index.row());

                if (block->row_type == RowType::kComment)
                {
                    const auto comment_block = std::static_pointer_cast<CommentBlock>(block);

                    data.setValue(QString(comment_block->text.c_str()));
                }
                else if (block->row_type == RowType::kCode)
                {
                    const auto code_block = std::static_pointer_cast<InstructionBlock>(block);

                    data.setValue(QString(code_block->token.token_text.c_str()));
                }
            }
            else
            {
                const auto row = blocks_.at(index.parent().row())->instruction_lines.at(index.row());

                if (row->row_type == RowType::kCode)
                {
                    const auto instruction = std::static_pointer_cast<InstructionRow>(row);

                    data.setValue(QString(instruction->op_code_token.token_text.c_str()));
                }
                else if (row->row_type == RowType::kComment)
                {
                    const auto comment = std::static_pointer_cast<CommentRow>(row);

                    data.setValue(QString(comment->text.c_str()));
                }
            }

            break;
        }
        case kOperands:
        {
            if (index.parent().isValid() && (blocks_.at(index.parent().row())->instruction_lines.at(index.row())->row_type == RowType::kCode))
            {
                const auto  instruction = std::static_pointer_cast<InstructionRow>(blocks_.at(index.parent().row())->instruction_lines.at(index.row()));
                const auto& operand_token_groups = instruction->operand_tokens;
                std::string operands_string      = "";

                for (size_t i = 0; i < operand_token_groups.size(); i++)
                {
                    const auto& operand_token_group = operand_token_groups.at(i);

                    for (size_t j = 0; j < operand_token_group.size(); j++)
                    {
                        const std::string& arg = operand_token_group[j].token_text;
                        operands_string += arg;

                        if (j != operand_token_group.size() - 1)
                        {
                            operands_string += kOperandTokenSpaceStdString;
                        }
                    }

                    if (i != operand_token_groups.size() - 1)
                    {
                        operands_string += kOperandDelimiterStdString;
                    }
                }

                data.setValue(QString(operands_string.c_str()));
            }
            break;
        }
        case kPcAddress:
        {
            if (index.parent().isValid() && (blocks_.at(index.parent().row())->instruction_lines.at(index.row())->row_type == RowType::kCode))
            {
                // Instruction line.

                const auto instruction = std::static_pointer_cast<InstructionRow>(blocks_.at(index.parent().row())->instruction_lines.at(index.row()));

                data.setValue(QString(instruction->pc_address.c_str()));
            }

            break;
        }
        case kBinaryRepresentation:
        {
            if (index.parent().isValid() && (blocks_.at(index.parent().row())->instruction_lines.at(index.row())->row_type == RowType::kCode))
            {
                // Instruction line.

                const auto instruction = std::static_pointer_cast<InstructionRow>(blocks_.at(index.parent().row())->instruction_lines.at(index.row()));

                data.setValue(QString(instruction->binary_representation.c_str()));
            }

            break;
        }
        default:
        {
            break;
        }
        }
        break;
    }
    case Qt::UserRole:
    {
        // Use UserRole to store data needed for delegates to custom paint instruction rows.
        //
        // Store tokens for instructions.
        // Store nothing for comments.

        if (!index.parent().isValid())
        {
            std::vector<Token> tokens;

            const auto block = blocks_.at(index.row());

            if (block->row_type == RowType::kCode)
            {
                const auto code_block = std::static_pointer_cast<InstructionBlock>(block);

                tokens.push_back(code_block->token);
            }

            data.setValue(tokens);
        }
        else
        {
            const auto row = blocks_.at(index.parent().row())->instruction_lines.at(index.row());

            if (row->row_type == RowType::kCode)
            {
                const auto instruction = std::static_pointer_cast<InstructionRow>(row);

                if ((index.column() == kOpCode))
                {
                    std::vector<Token> tokens;

                    tokens.push_back(instruction->op_code_token);

                    data.setValue(tokens);
                }
                else if ((index.column() == kOperands))
                {
                    data.setValue(instruction->operand_tokens);
                }
            }
        }

        break;
    }
    case kLabelBranchRole:
    {
        bool is_code_block_label_branch_target = false;

        if (!index.parent().isValid())
        {
            const auto block = blocks_.at(index.row());

            if (block->row_type == RowType::kCode)
            {
                const auto code_block             = std::static_pointer_cast<InstructionBlock>(block);
                is_code_block_label_branch_target = !code_block->mapped_branch_instructions.empty();
            }
        }

        data.setValue(is_code_block_label_branch_target);
        break;
    }
    case kBranchIndexRole:
    {
        QVector<QModelIndex> branch_label_indices;

        if (index.column() == kOpCode)
        {
            if (!index.parent().isValid())
            {
                const auto block = blocks_.at(index.row());

                if (block->row_type == RowType::kCode)
                {
                    const auto code_block = std::static_pointer_cast<InstructionBlock>(block);

                    for (const auto& mapped_branch_instruction : code_block->mapped_branch_instructions)
                    {
                        const int         label_code_block_index  = mapped_branch_instruction.first;
                        const int         label_instruction_index = mapped_branch_instruction.second;
                        const QModelIndex branch_label_index      = this->index(label_instruction_index, 0, this->index(label_code_block_index, 0));
                        branch_label_indices.push_back(branch_label_index);
                    }
                }
            }
        }
        else if (index.column() == kOperands)
        {
            if (index.parent().isValid())
            {
                // Instruction line.

                const auto row = blocks_.at(index.parent().row())->instruction_lines.at(index.row());

                if (row->row_type == RowType::kCode)
                {
                    const auto instruction = std::static_pointer_cast<const InstructionRow>(row);

                    if (!instruction->operand_tokens.empty() && !instruction->operand_tokens.front().empty())
                    {
                        // Check the first token in the first operand.

                        const auto& token = instruction->operand_tokens.front().front();

                        if (token.type == TokenType::kBranchLabelType && token.start_register_index != -1)
                        {
                            const QModelIndex label_index = this->index(token.start_register_index, 0);
                            branch_label_indices.push_back(label_index);
                        }
                    }
                }
            }
        }

        data.setValue(branch_label_indices);
        break;
    }
    case kLineEnabledRole:
    {
        bool line_enabled = true;

        if (index.parent().isValid())
        {
            const auto row = blocks_.at(index.parent().row())->instruction_lines.at(index.row());

            if (row->row_type == RowType::kCode)
            {
                const auto instruction = std::static_pointer_cast<InstructionRow>(row);
                line_enabled           = instruction->enabled;
            }
        }

        data.setValue(line_enabled);
        break;
    }
    case kRowTypeRole:
    {
        RowType row_type = RowType::kRowCount;

        if (!index.parent().isValid())
        {
            const auto block = blocks_.at(index.row());
            row_type         = block->row_type;
        }
        else
        {
            const auto row = blocks_.at(index.parent().row())->instruction_lines.at(index.row());
            row_type       = row->row_type;
        }

        data.setValue(row_type);
        break;
    }
    default:
    {
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

    if (blocks_.empty())
    {
        return;
    }

    int code_block_index = 0;

    for (auto& code_block : blocks_)
    {
        line_number_corresponding_indices_.emplace_back(-1, code_block_index);

        int instruction_index = 0;

        for (auto instruction : code_block->instruction_lines)
        {
            line_number_corresponding_indices_.emplace_back(code_block_index, instruction_index++);

            if (instruction->row_type == RowType::kComment)
            {
                // Don't force comments to fit in the op code column.
                continue;
            }

            const auto instruction_line = std::static_pointer_cast<InstructionRow>(instruction);

            max_op_code_length    = std::max(max_op_code_length, static_cast<uint32_t>(instruction_line->op_code_token.token_text.size()));
            max_pc_address_length = std::max(max_pc_address_length, static_cast<uint32_t>(instruction_line->pc_address.length()));

            std::string operands;

            const auto number_operand_groups = instruction_line->operand_tokens.size();

            for (size_t i = 0; i < number_operand_groups; i++)
            {
                const auto& operand_group            = instruction_line->operand_tokens.at(i);
                const auto  number_operands_in_group = operand_group.size();

                for (size_t j = 0; j < number_operands_in_group; j++)
                {
                    const auto& operand = operand_group.at(j);

                    operands += operand.token_text;

                    if (j < number_operands_in_group - 1)
                    {
                        operands += kOperandTokenSpaceStdString;
                    }
                }

                if (i < number_operand_groups - 1)
                {
                    operands += kOperandDelimiterStdString;
                }
            }

            max_operand_length = std::max(max_operand_length, static_cast<uint32_t>(operands.size()));
            max_binary_representation_length =
                std::max(max_binary_representation_length, static_cast<uint32_t>(instruction_line->binary_representation.size()));
        }

        code_block_index++;
    }

    const int line_number_text_length = QString(QString::number(blocks_.back()->instruction_lines.back()->line_number) + kColumnPadding).size();
    max_op_code_length += kOpCodeColumnIndent.size();

    const auto line_number_length           = line_number_text_length * fixed_font_character_width_;
    const auto pc_address_length            = max_pc_address_length * fixed_font_character_width_;
    const auto op_code_length               = max_op_code_length * fixed_font_character_width_;
    const auto operand_length               = max_operand_length * fixed_font_character_width_;
    const auto binary_representation_length = max_binary_representation_length * fixed_font_character_width_;

    column_widths_[kLineNumber]           = line_number_length;
    column_widths_[kPcAddress]            = pc_address_length;
    column_widths_[kOpCode]               = op_code_length;
    column_widths_[kOperands]             = operand_length;
    column_widths_[kBinaryRepresentation] = binary_representation_length;
}

QSize SharedIsaItemModel::ColumnSizeHint(int column_index, SharedIsaTreeView* tree) const
{
    QSize size_hint(0, 0);

    if (column_index < 0 || column_index >= kColumnCount)
    {
        return size_hint;
    }

    size_hint.setHeight(QFontMetricsF(fixed_font_, tree).height() + 2.0); // Add arbitrary spacing found in Qt.
    size_hint.setWidth(column_widths_[column_index]);

    return size_hint;
}

void SharedIsaItemModel::SetFixedFont(const QFont& fixed_font, SharedIsaTreeView* tree)
{
    fixed_font_ = fixed_font;

    QFontMetricsF font_metrics(fixed_font_, tree);

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

void SharedIsaItemModel::ClearBranchInstructionMapping()
{
    for (auto block : blocks_)
    {
        if (block->row_type != RowType::kCode)
        {
            continue;
        }

        auto code_block = std::static_pointer_cast<InstructionBlock>(block);

        code_block->mapped_branch_instructions.clear();
    }
}

void SharedIsaItemModel::MapBlocksToBranchInstructions()
{
    if (blocks_.empty())
    {
        return;
    }

    ClearBranchInstructionMapping();

    code_block_label_to_index_.clear();

    // Build map of code block label -> code block index.

    for (size_t block_index = 0; block_index < blocks_.size(); block_index++)
    {
        const auto block = blocks_.at(block_index);

        if (block->row_type != RowType::kCode)
        {
            continue;
        }

        const auto code_block                                    = std::static_pointer_cast<const InstructionBlock>(block);
        code_block_label_to_index_[code_block->token.token_text] = code_block->position;
    }

    // Use map to assign block's their corresponding branch instructions and vice versa.

    for (size_t block_index = 0; block_index < blocks_.size(); block_index++)
    {
        const auto block = blocks_.at(block_index);

        if (block->row_type != RowType::kCode)
        {
            continue;
        }

        const auto code_block = std::static_pointer_cast<InstructionBlock>(block);

        for (size_t instruction_index = 0; instruction_index < code_block->instruction_lines.size(); instruction_index++)
        {
            const auto row = code_block->instruction_lines.at(instruction_index);

            if (row->row_type != RowType::kCode)
            {
                continue;
            }

            auto       instruction  = std::static_pointer_cast<InstructionRow>(row);
            const auto op_code_text = instruction->op_code_token.token_text;
            const bool is_branch =
                ((op_code_text.find(kUnconditionalBranchString) != std::string::npos) || (op_code_text.find(kConditionalBranchString) != std::string::npos));

            if (is_branch && (!instruction->operand_tokens.empty()) && (!instruction->operand_tokens.front().empty()))
            {
                // Assume branch target is first operand of first operand group.

                const auto map_iter = code_block_label_to_index_.find(instruction->operand_tokens.front().front().token_text);

                if (map_iter != code_block_label_to_index_.end())
                {
                    const auto branch_target_block_index = map_iter->second;
                    const auto branch_target_block       = blocks_.at(branch_target_block_index);
                    auto       branch_target_code_block = std::static_pointer_cast<SharedIsaItemModel::InstructionBlock>(blocks_.at(branch_target_block_index));

                    // Code block remembers which branch instruction targeted it.
                    branch_target_code_block->mapped_branch_instructions.emplace_back(std::make_pair(block_index, instruction_index));

                    // Branch instruction remembers which code block is its target.
                    instruction->operand_tokens.front().front().start_register_index = branch_target_block_index;
                }
            }
        }
    }

    code_block_label_to_index_.clear();
}

SharedIsaItemModel::Row::Row(RowType type, uint32_t line)
    : row_type(type)
    , line_number(line)
{
}

SharedIsaItemModel::Row::~Row()
{
}

SharedIsaItemModel::CommentRow::CommentRow(uint32_t line, std::string comment)
    : Row(RowType::kComment, line)
    , text(comment)
{
}

SharedIsaItemModel::CommentRow::~CommentRow()
{
}

SharedIsaItemModel::InstructionRow::InstructionRow(uint32_t line, std::string op, std::string address, std::string representation)
    : Row(RowType::kCode, line)
    , pc_address(address)
    , binary_representation(representation)
    , enabled(true)
{
    op_code_token.token_text = op;
}

SharedIsaItemModel::InstructionRow::~InstructionRow()
{
}

SharedIsaItemModel::Block::Block(RowType type, int block_position, uint32_t shader_line_number)
    : row_type(type)
    , position(block_position)
    , line_number(shader_line_number)
{
}

SharedIsaItemModel::Block::~Block()
{
}

SharedIsaItemModel::CommentBlock::CommentBlock(int block_position, uint32_t shader_line_number, std::string comment_text)
    : Block(RowType::kComment, block_position, shader_line_number)
    , text(comment_text)
{
}

SharedIsaItemModel::CommentBlock::~CommentBlock()
{
}

SharedIsaItemModel::InstructionBlock::InstructionBlock(int block_position, uint32_t shader_line_number, std::string block_label)
    : Block(RowType::kCode, block_position, shader_line_number)
{
    token.token_text = block_label;
}

SharedIsaItemModel::InstructionBlock::~InstructionBlock()
{
}