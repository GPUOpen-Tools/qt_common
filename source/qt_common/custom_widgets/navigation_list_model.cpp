//=============================================================================
/// Copyright (c) 2020-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation for a navigation list model
//=============================================================================

#include "navigation_list_model.h"

NavigationListModel::NavigationListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

NavigationListModel::NavigationListModel(QStringList entries, QObject* parent)
    : QAbstractListModel(parent)
    , item_list_(entries)
{
}

NavigationListModel::~NavigationListModel()
{
}

void NavigationListModel::AddEntry(const QString& entry)
{
    beginResetModel();

    item_list_.append(entry);

    endResetModel();
}

QVariant NavigationListModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    if (role == Qt::DisplayRole)
    {
        return item_list_.at(row);
    }

    return QVariant();
}

Qt::ItemFlags NavigationListModel::flags(const QModelIndex& index) const
{
    return QAbstractItemModel::flags(index);
}

int NavigationListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return item_list_.count();
}

bool NavigationListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    int row = index.row();
    if (role == Qt::EditRole)
    {
        QString name = value.toString();

        // Update the value from editor into blacklist
        item_list_[row] = value.toString();

        return true;
    }

    return false;
}

bool NavigationListModel::removeRows(int row, int count, const QModelIndex& parent)
{
    int last = row + (count - 1);
    beginRemoveRows(parent, row, last);

    // Remove rows from blacklist
    for (int i = row; i <= last; i++)
    {
        item_list_.removeAt(row);
    }

    endRemoveRows();

    return true;
}

bool NavigationListModel::insertRows(int row, int count, const QModelIndex& parent)
{
    int end = row + count;
    beginInsertRows(parent, row, end);

    for (int i = row; i < end; i++)
    {
        item_list_.insert(i, "");
    }

    endInsertRows();

    return true;
}
