//=============================================================================
/// Copyright (c) 2019-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Definition of a custom TreeView implementation.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_TREE_VIEW_H_
#define QTCOMMON_CUSTOM_WIDGETS_TREE_VIEW_H_

#include <QTreeView>

class TreeView : public QTreeView
{
    Q_OBJECT

public:
    /// Explicit constructor.
    /// \param parent The widget's parent.
    explicit TreeView(QWidget* parent = nullptr);

    /// Destructor
    virtual ~TreeView();

    /// Checks if view is currently editing specified index
    /// Workaround for bug in Qt with QStyle::State_Editing
    /// not being set correctly on indices.
    /// \param index The index to check editing for.
    /// \return true is view is editing index.
    bool IsEditing(const QModelIndex& index) const;

private:
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_TREE_VIEW_H_
