//=============================================================================
// Copyright (c)2022-2023 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Shared isa proxy model implementation.
//=============================================================================

#include "shared_isa_proxy_model.h"

#include "shared_isa_item_model.h"

SharedIsaProxyModel::SharedIsaProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    // Most of the columns should be visible by default.
    for (int i = 0; i < SharedIsaItemModel::kColumnCount; i++)
    {
        visible_columns_[i] = true;

        column_order_[i] = i;
    }

    // But two should be hidden, since they are used less frequently.
    visible_columns_[SharedIsaItemModel::kPcAddress]            = false;
    visible_columns_[SharedIsaItemModel::kBinaryRepresentation] = false;
}

SharedIsaProxyModel::~SharedIsaProxyModel()
{
}

void SharedIsaProxyModel::SetColumnVisibility(uint32_t column, bool visibility, QHeaderView* header)
{
    if (column >= visible_columns_.size())
    {
        return;
    }

    visible_columns_[column] = visibility;

    if (!visibility && header != nullptr)
    {
        int proxy_column = mapFromSource(sourceModel()->index(0, column)).column();
        int visual_index = header->visualIndex(proxy_column);

        column_order_[column] = visual_index;
    }

    invalidateFilter();

    if (visibility && header != nullptr)
    {
        int proxy_column = mapFromSource(sourceModel()->index(0, column)).column();
        if (column_order_[column] >= columnCount())
        {
            column_order_[column] = columnCount() - 1;
        }
        header->moveSection(proxy_column, column_order_[column]);
    }
}

bool SharedIsaProxyModel::filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const
{
    Q_UNUSED(source_parent);

    if (source_column >= static_cast<int>(visible_columns_.size()))
    {
        return true;
    }

    return visible_columns_[source_column];
}