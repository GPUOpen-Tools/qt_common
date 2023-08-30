//=============================================================================
// Copyright (c) 2022-2023 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Declaration for a shared isa item model.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_ITEM_MODEL_H_
#define QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_ITEM_MODEL_H_

#include <QAbstractItemModel>
#include <QFont>
#include <QColor>

#include <array>
#include <map>
#include <string>
#include <utility>
#include <vector>

/// @brief SharedIsaItemModel is an item model that stores code block labels as parent nodes
///        and individual instructions as child nodes.
class SharedIsaItemModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    static const QString kColumnPadding;       ///< Padding for columns.
    static const QString kOpCodeColumnIndent;  ///< Indent for op codes column.
    static const QString kOperandDelimiter;    ///< Delimiter to separate operands.

    /// @brief Predefined columns.
    enum Columns
    {
        kLineNumber = 0,
        kPcAddress,
        kOpCode,
        kOperands,
        kBinaryRepresentation,
        kColumnCount
    };

    /// @brief Extend user roles to store extra info for code block label to branch instruction mapping, and
    ///        whether a given instruction should be color coded.
    enum UserRoles
    {
        kLabelBranchRole = Qt::UserRole + 1,  // Store true if the index is a code block label and the target of a branch instruction, otherwise false.
        kBranchIndexRole,                     // Store a list of source model indices of the corresponding branch instruction or label.
        kLineEnabledRole,                     // true if the index is an instruction that should be color coded, false if it should not be color coded.
        kUserRolesCount
    };

    /// @brief Predefined types to assist formatting and interacting with tokens.
    enum class TokenType
    {
        kLabelType = 0,       ///< Code block label in op code column.
        kBranchLabelType,     ///< Label targeted by a branch instruction in operands column.
        kScalarRegisterType,  ///< Scalar operand.
        kVectorRegisterType,  ///< Vector operand.
        kConstantType,        ///< Constant operand.
        kCommentType,         ///< An application specific comment created for other isa tokens.
        kTypeCount
    };

    /// @brief Token is a convenience struct intended to act as a temporary representation of a single word of isa.
    ///        It assists color coding and selecting/highlighting.
    typedef struct Token
    {
        std::string token_text;            ///< The token in text form.
        TokenType   type;                  ///< The type of this token.
        int         start_register_index;  ///< The starting register index if this token represents a register.
        int         end_register_index;    ///< The ending register index if this token represents a register.
        int         x_position_start;      ///< The token's starting x view position.
        int         x_position_end;        ///< The token's ending x view position.
        bool        is_selectable;         ///< true if the token can be selected, false otherwise.

        /// @brief Constructor; create an empty token.
        Token()
        {
            Clear();
        }

        /// @brief Clear this token so it is empty and does not represent anything.
        void Clear()
        {
            token_text           = "";
            type                 = TokenType::kTypeCount;
            start_register_index = -1;
            end_register_index   = -1;
            x_position_start     = -1;
            x_position_end       = -1;
            is_selectable        = false;
        }
    } Token;

    /// @brief Constructor.
    ///
    /// @param [in] parent The parent object.
    SharedIsaItemModel(QObject* parent = nullptr);

    /// @brief Destructor.
    ~SharedIsaItemModel();

    /// @brief Override column count to provide a fixed set of shared columns.
    ///
    /// @param [in] parent The parent index.
    ///
    /// @return The number of columns for the given index.
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    /// @brief Override row count to provide the number of dynamic rows.
    ///
    /// The number of top level rows in this model is equivalent to the number of code blocks in this model.
    /// The number of rows in any given code block is the number of instructions in that code block.
    ///
    /// @param [in] parent The parent index.
    ///
    /// @return The number of rows for the given index.
    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE final;

    /// @brief Override index to generate the correct index for the given row, column, and parent index.
    ///
    /// Top level code blocks are assigned an index with no internal data.
    /// Child level instructions are assigned an index with internal data that is a pointer to their parent code block.
    ///
    /// @param [in] row    The row.
    /// @param [in] column The column.
    /// @param [in] parent The parent index.
    ///
    /// @return The generated index.
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    /// @brief Override parent to assign parents.
    ///
    /// Code blocks are parented to the root node.
    /// Instructions are parented to their code block.
    ///
    /// @param [in] index The index to generate a parent for.
    ///
    /// @return The parent index.
    virtual QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;

    /// @brief Override headerData to provide data to display in the header.
    ///
    /// Only defines display role text to display for a given column.
    ///
    /// @param [in] section     The section number.
    /// @param [in] orientation The orientation.
    /// @param [in] role        The role.
    ///
    /// @return The variant to use as header data.
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    /// @brief Override data to provide data for a given index.
    ///
    /// @param [in] index The index.
    /// @param [in] role  The role.
    ///
    /// @return The variant to use as data.
    virtual QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;

    /// @brief UpdateData is a pure virtual function intended to be overridden by child classes to update the internal state of this model.
    ///
    /// This is intended to be the primary interface function to update the isa text in this model.
    /// A (void*) parameter is supplied so that an application may pass in any type at their convenience.
    ///
    /// @param [in] data A pointer to application metadata.
    virtual void UpdateData(void* data) = 0;

    /// @brief Cache the sizes of shared columns by using the text in this model and the font being used by an isa tree.
    ///
    /// Also, map line numbers to their source code block indices.
    virtual void CacheSizeHints();

    /// @brief Get the cached column size for the requested column index.
    ///
    /// @param [in] column_index The column index.
    ///
    /// @return The cached column size.
    virtual QSize ColumnSizeHint(int column_index) const;

    /// @brief Set the fixed font being used by an isa tree; use it to cache column sizes.
    ///
    /// @param [in] fixed_font The fixed font to use.
    void SetFixedFont(const QFont& fixed_font);

    /// @brief Get the source model index that corresponds to the provided line number.
    ///
    /// @param [in] line_number The line number.
    ///
    /// @return The source model index that corresponds to the provided line number.
    QModelIndex GetLineNumberModelIndex(int line_number);

    /// @brief Get the number of isa lines stored in this model; code block labels and instructions both count towards the line count.
    ///
    /// @return The number of lines in the model.
    inline int GetLineCount() const
    {
        return static_cast<int>(line_number_corresponding_indices_.size());
    }

protected:
    /// @brief Predefined row types.
    enum class RowType
    {
        kCodeBlock = 0,
        kInstruction,
        kComment,
        kRowCount
    };

    /// @brief Map code blocks indices to corresponding jump instruction indices.
    virtual void MapBlocksToBranchInstructions();

    /// @brief InstructionLine is a convenience struct meant to represent a single instruction line.
    typedef struct InstructionLine
    {
        RowType                         row_type;        ///< The type of this row.
        uint64_t                        line_number;     ///< Line # relative to the entire shader.
        std::string                     op_code;         ///< The opcode text in this instruction.
        std::vector<std::string>        operands;        ///< The operand text in this instruction.
        Token                           op_code_token;   ///< This instruction's opcode's token.
        std::vector<std::vector<Token>> operand_tokens;  ///< This instruction's operands' tokens; tokens belonging to the same operand are grouped together.
        std::string                     pc_address;      ///< The pc address text of this instruction.
        std::string                     binary_representation;  ///< The binary representation text of this instruction.
        bool                            enabled;                ///< true if this instruction should be color coded, false otherwise.
    } InstructionLine;

    /// @brief CodeBlock is a convenience struct meant to represent an isa code block.
    typedef struct CodeBlock
    {
        RowType                          row_type;                    ///< The type of this row.
        Token                            token;                       ///< This code block's label's token.
        int                              position;                    ///< This code block's index into this model's data structure.
        uint64_t                         line_number;                 ///< Line # relative to the entire shader.
        std::vector<InstructionLine>     instruction_lines;           ///< All instruction lines that belong to this code block.
        std::vector<std::pair<int, int>> mapped_branch_instructions;  ///< Map this code block to corresponding jump instruction indices.
    } CodeBlock;

    std::vector<CodeBlock> code_blocks_;  ///< Isa stored in this model as a container of a convenience data structure.

    QFont  fixed_font_;                  ///< A fixed font set by an application to assist caching size hints for columns.
    int    fixed_font_character_width_;  ///< Cache the width of a single character of the fixed font.

private:
    std::array<uint32_t, kColumnCount>    column_widths_ = {0, 0, 0, 0, 0};                                                    ///< Cached column widths.
    std::array<std::string, kColumnCount> column_names_  = {"", "PC address", "Opcode", "Operands", "Binary representation"};  ///< Predefined column headers.

    std::vector<std::pair<int, int>>
        line_number_corresponding_indices_;  ///< Map line numbers to their corresponding source model indices; Line number to <parent row, child row>.
};

Q_DECLARE_METATYPE(SharedIsaItemModel::Token);

#endif  // QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_ITEM_MODEL_H_
