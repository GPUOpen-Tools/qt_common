//=============================================================================
/// Copyright (c) 2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header file for a scaled QTableView
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SCALED_TABLE_VIEW_H_
#define QTCOMMON_CUSTOM_WIDGETS_SCALED_TABLE_VIEW_H_

#include "scaled_header_view.h"

#include <QTableView>

class QAbstractItemModel;
class QPaintEvent;

/// Default additional column padding in pixels.
static const int kScaledTableViewDefaultColumnPadding = 10;

/// This class implements a Table View
class ScaledTableView : public QTableView
{
    Q_OBJECT

public:
    /// Explicit constructor
    /// \param parent The parent widget.
    explicit ScaledTableView(QWidget* parent = nullptr);

    /// Destructor
    virtual ~ScaledTableView();

    /// Sets the default properties of the table.
    /// \param padding A pixel-based padding to help separate the columns with whitespace.
    void SetDefaultProperties(int padding = kScaledTableViewDefaultColumnPadding);

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
    ///  Adjust table sizes, column widths and row heights based on DPI scale
    ///  and data contents.
    void OnScaleFactorChanged();

private:

    /// Additional padding in pixels after the text to space out the columns.
    int column_padding_;

    /// Pointer to horizontal ScaledHeaderView
    /// Needed so the column padding can be passed on to the header.
    ScaledHeaderView* horizontal_header_;

    /// Pointer to vertical ScaledHeaderView
    /// Needed so the column padding can be passed on to the header.
    ScaledHeaderView* vertical_header_;
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SCALED_TABLE_VIEW_H_
