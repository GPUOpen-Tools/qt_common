//=============================================================================
/// Copyright (c) 2020-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header for a navigation list model
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_NAVIGATION_LIST_MODEL_H_
#define QTCOMMON_CUSTOM_WIDGETS_NAVIGATION_LIST_MODEL_H_

#include <QAbstractListModel>

class NavigationListModel : public QAbstractListModel
{
public:
    /// Constructor.
    /// \param parent The view's parent.
    NavigationListModel(QObject* parent = nullptr);

    /// Constructor.
    /// \param entries The string labels for the navigation options.
    /// \param parent The view's parent.
    NavigationListModel(QStringList entries, QObject* parent = nullptr);

    /// Destructor
    virtual ~NavigationListModel() Q_DECL_OVERRIDE;

    /// \brief Adds entry to model
    /// \param entry The entry to add
    void AddEntry(const QString& entry);

protected:
    /// \brief QAbstractListModel::data() implementation
    /// \param index The index to query data for
    /// \param role The data role
    /// \return variant data for specified role
    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;

    /// \brief QAbstractListModel::flags() implementation
    /// \param index The index to query flags for
    /// \return the item flags for index
    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;

    /// \brief QAbstractListModel::rowCount() implementation
    /// \param parent The parent index
    /// \return number of rows in list
    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    /// \brief QAbstractListModel::setData() implementation
    /// \param index The index to set internal data for
    /// \param value The new data value
    /// \param role The role to set new data for
    /// \return True if internal data changed
    bool setData(const QModelIndex& index, const QVariant& value, int role) Q_DECL_OVERRIDE;

    /// \brief QAbstractListModel::removeRows() implementation
    /// \param row The row to start removal from
    /// \param count The number of subsequent rows to remove
    /// \param parent The parent index
    /// \return True is rows removed
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;

    /// \brief QAbstractListModel::insertRows() implementation
    /// \param row The row to start insert from
    /// \param count The number of rows to insert
    /// \param parent The parent index
    /// \return True is rows inserted
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;

protected:
    QStringList item_list_;  ///< navigation list entries
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_NAVIGATION_LIST_MODEL_H_
