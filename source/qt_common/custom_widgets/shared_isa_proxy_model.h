//=============================================================================
// Copyright (c) 2022-2023 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Header for a shared isa proxy model.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_PROXY_MODEL_H_
#define QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_PROXY_MODEL_H_

#include <array>

#include <QHeaderView>
#include <QSortFilterProxyModel>

#include "shared_isa_item_model.h"

/// @brief SharedIsaProxyModel is a filter model meant to filter default columns for a SharedIsaItemModel.
///
/// It filters out SharedIsaItemModel columns set to be invisible via the gui.
class SharedIsaProxyModel : public QSortFilterProxyModel
{
public:
    /// @brief Constructor; default all columns to visible.
    ///
    /// @param [in] parent The parent object.
    explicit SharedIsaProxyModel(QObject* parent = nullptr);

    /// @brief Destructor.
    virtual ~SharedIsaProxyModel();

    /// @brief Change the visibility of a column and invalidate this model.
    ///
    /// @param [in] column     The SharedIsaItemModel column to change.
    /// @param [in] visibility true to make the column visible, false to make it invisible.
    void SetColumnVisibility(uint32_t column, bool visibility, QHeaderView* header = nullptr);

protected:
    /// @brief Override filterAcceptsColumn to filter columns set to be invisible.
    ///
    /// @param [in] source_column The source column.
    /// @param [in] source_parent The source parent index.
    ///
    /// @return true if the column is marked as visible, false otherwise.
    virtual bool filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const Q_DECL_OVERRIDE;

private:
    std::array<bool, SharedIsaItemModel::kColumnCount> visible_columns_;  ///< Keep track of which columns should be visible.
    int column_order_[SharedIsaItemModel::kColumnCount];                  ///< Keep track of where a hidden column should be placed when it is reshown.
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_PROXY_MODEL_H_
