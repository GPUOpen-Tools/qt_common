//=============================================================================
// Copyright (c) 2022-2024 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Declaration for a shared isa item delegate.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_ITEM_DELEGATE_H_
#define QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_ITEM_DELEGATE_H_

#include <QModelIndex>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>

#include "shared_isa_item_model.h"
#include "shared_isa_proxy_model.h"
#include "shared_isa_tree_view.h"

/// @brief SharedIsaItemDelegate is a styled delegate to be used with the SharedIsaTreeView.
///        It custom paints isa text and handles user interaction.
class SharedIsaItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    /// @brief Constructor.
    ///
    /// @param [in] view   The corresponding tree view.
    /// @param [in] parent The parent object.
    SharedIsaItemDelegate(SharedIsaTreeView* view, QObject* parent = nullptr);

    /// @brief Destructor.
    ~SharedIsaItemDelegate();

    /// @brief Override editor event in order to track mouse moves and mouse clicks over code block labels and selectable tokens.
    ///
    /// @param [in] event  The event that triggered editing.
    /// @param [in] model  The model.
    /// @param [in] option The option used to render the item.
    /// @param [in] index  The index of the item.
    ///
    /// @return true.
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) Q_DECL_OVERRIDE;

    /// @brief Override paint to custom render isa text.
    ///
    /// @param [in] painter     The painter.
    /// @param [in] option      The option used to render the item.
    /// @param [in] model_index The index to render.
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& model_index) const Q_DECL_OVERRIDE;

    /// @brief Override sizeHint to cache text width to improve performance.
    ///
    /// @param [in] option Qt style option parameter.
    /// @param [in] index  Qt model index parameter.
    ///
    /// @return The size hint of the provided model index.
    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;

protected:
    /// @brief Determines if the source model index is at the top of its corresponding tree viewport and is a child row.
    ///
    /// If the index is at the top and is a child, the parent label will be pinned there instead.
    ///
    /// @param [in]  source_model_index               The source model index.
    /// @param [in]  proxy_model_index                The proxy model index.
    /// @param [in]  proxy_index_y_position           The y position of the proxy model index.
    /// @param [out] pinned_label_line_number_column  true if the index is at the top of its tree, is a child index, and is the line number column.
    ///
    /// @return true if the index is a child index and at the top of the corresponding tree view, false if not.
    bool CodeBlockLabelPinnedToTop(const QModelIndex& source_model_index,
                                   const QModelIndex& proxy_model_index,
                                   int&               proxy_index_y_position,
                                   bool&              pinned_label_line_number_column) const;

    SharedIsaTreeView* view_;  ///< The corresponding tree view.

private:
    /// @brief Help navigating from labels to branches by accomodating for column span when calculating x position relative to a column.
    ///
    /// @param [in]  index            The view index to check.
    /// @param [in]  proxy            The proxy model.
    /// @param [out] source_index     The source index to check; will be changed to the op code source index if the given index spans columns.
    /// @param [out] local_x_position The x position of a mouse event; will be changed if the given index spans columns to be relative to the op code column.
    void AdjustXPositionForSpannedColumns(const QModelIndex& index, const QSortFilterProxyModel* proxy, QModelIndex& source_index, int& local_x_position);

    /// @brief Helper function to help determine if a selectable token is under the mouse.
    ///
    /// @param [in]  source_index                The source model index that the mouse move event occurred under.
    /// @param [in]  local_x_position            The local viewport x position, relative to the index, of the mouse move event.
    /// @param [out] isa_token_under_mouse       The token text that is underneath the mouse, if any.
    /// @param [out] isa_token_under_mouse_index The token index that is underneath the mouse, if any.
    ///
    /// @return true if a token that is eligible to be selected was found underneath the mouse, false otherwise.
    bool SetSelectableTokenUnderMouse(const QModelIndex&         source_index,
                                      const int                  local_x_position,
                                      SharedIsaItemModel::Token& isa_token_under_mouse,
                                      int&                       isa_token_under_mouse_index);

    /// @brief Helper function to help change the cursor when a branch label token is under the mouse.
    ///
    /// @param [in]  source_index     The source model index that the mouse move event occurred under.
    /// @param [in]  local_x_position The local viewport x position, relative to the index, of the mouse move event.
    bool SetBranchLabelTokenUnderMouse(const QModelIndex& source_index, const int local_x_position);

    /// @brief Helper function to help paint a highlight behind the token that is currently underneath the mouse cursor.
    ///        This function first checks if the requested token is underneath the mouse, and if it is, paints a highlight behind it.
    ///
    /// @param [in] token               The token to check.
    /// @param [in] isa_token_rectangle The token's rectangle.
    /// @param [in] painter             The painter.
    /// @param [in] font_metrics        The font metrics.
    /// @param [in] code_block_index    The token's code block index.
    /// @param [in] instruction_index   The token's instruction index.
    /// @param [in] token_index         The token's index.
    void PaintTokenHighlight(const SharedIsaItemModel::Token& token,
                             const QRect&                     isa_token_rectangle,
                             QPainter*                        painter,
                             const QFontMetrics&              font_metrics,
                             int                              code_block_index,
                             int                              instruction_index,
                             int                              token_index) const;

    /// @brief Helper function to paint the text of a list of isa tokens or isa comments.
    ///
    /// @param [in] painter         The QPainter that will be used for painting.
    /// @param [in] option          The style option for determining font metrics.
    /// @param [in] index           The model index of the token.
    /// @param [in] token_rectangle The rectangle where the token text will be drawn.
    /// @param [in] tokens          The list of tokens to be painted.
    /// @param [in] token_index     The starting index of the token in the double vector of tokens. Zero if there is only a singe vector.
    /// @param [in] is_comment      true if painting comment text, false if painting an instruction's tokens.
    ///
    /// @return A pair of the final token index and text rectangle for when painting a double vector of tokens.
    std::pair<int, QRect> PaintText(QPainter*                              painter,
                                    const QStyleOptionViewItem&            option,
                                    const QModelIndex&                     index,
                                    QRect                                  token_rectangle,
                                    std::vector<SharedIsaItemModel::Token> tokens,
                                    int                                    token_index,
                                    bool                                   is_comment) const;

    /// @brief Helper function to paint an isa opcode or isa comments in a spanned column.
    ///
    /// @param [in] painter      The QPainter that will be used for painting.
    /// @param [in] option       The style option for determining font metrics.
    /// @param [in] source_index The source index where the data to be painted is.
    /// @param [in] proxy_index  The proxy index of where the painting should occur if it is not a comment.
    void PaintSpanned(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& source_index, QModelIndex proxy_index) const;

    SharedIsaItemModel::Token mouse_over_isa_token_;  ///< Track the token that the mouse is over.
    SharedIsaItemModel::Token selected_isa_token_;    ///< Track the selected token.

    int mouse_over_code_block_index_;   ///< Track the code block index that the mouse is over.
    int mouse_over_instruction_index_;  ///< Track the instruction index that the mouse is over.
    int mouse_over_token_index_;        ///< Track the token index that the mouse is over.
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_ITEM_DELEGATE_H_
