//=============================================================================
/// Copyright (c) 2019-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a custom TreeView implementation
//=============================================================================

#include "tree_view.h"

TreeView::TreeView(QWidget* parent)
    : QTreeView(parent)
{
}

TreeView::~TreeView()
{
}

bool TreeView::IsEditing(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return false;
    }

    return state() & QAbstractItemView::EditingState && currentIndex() == index;
}
