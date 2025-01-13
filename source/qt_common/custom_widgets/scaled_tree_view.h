//=============================================================================
/// Copyright (c) 2020-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header file for a scaled QTreeView
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SCALED_TREE_VIEW_H_
#define QTCOMMON_CUSTOM_WIDGETS_SCALED_TREE_VIEW_H_

#include "scaled_header_view.h"

#include <QTreeView>

/// Default additional column padding in pixels.
static const int kScaledTreeViewDefaultColumnPadding = 10;

/// This class implements a Tree View
class ScaledTreeView : public QTreeView
{
    Q_OBJECT

public:
    /// Explicit constructor
    /// \param parent The parent widget.
    explicit ScaledTreeView(QWidget* parent = nullptr);

    /// Destructor
    virtual ~ScaledTreeView();

    /// Sets up the default properties of the tree.
    /// \param padding A pixel-based padding to help separate the columns with whitespace.
    void SetDefaultProperties(int padding = kScaledTreeViewDefaultColumnPadding);

    /// Set additional padding after the text to space out the columns.
    /// \param padding Padding width in pixels.
    void SetColumnPadding(int padding);

    /// Sets the column width to the specified width in em's.
    /// This is a pass-through to the horizontal header.
    void SetColumnWidthEms(const int column, const int em_count);

protected:
    /// Reimplemented to add column padding to the sizeHint.
    /// \param column The column index.
    /// \return Desired width of the column in pixels.
    virtual int sizeHintForColumn(int column) const Q_DECL_OVERRIDE;

protected slots:
    ///  Adjust view sizes, column widths and row heights based on DPI scale
    ///  and data contents.
    void ScaleFactorChanged();

private:
    /// Additional padding in pixels after the text to space out the columns.
    int column_padding_;

    /// Pointer to the ScaledHeaderView.
    /// Needed so the column padding can be passed on to the header.
    ScaledHeaderView* horizontal_header_;
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SCALED_TREE_VIEW_H_
