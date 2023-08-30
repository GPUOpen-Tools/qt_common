//=============================================================================
/// Copyright (c) 2019-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header file for a scaled QHeaderView
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SCALED_HEADER_VIEW_H_
#define QTCOMMON_CUSTOM_WIDGETS_SCALED_HEADER_VIEW_H_

#include <QHeaderView>

/// ScaledHeaderView allows the header to support an additional column margin
/// on the right side of the header text to help space out the columns.
/// The ScaledHeaderView properly supports DPI scale changes and font changes
/// when it is part of a ScaledTableView. It will not support these features
/// when added to a standard QTableView.
class ScaledHeaderView : public QHeaderView
{
    Q_OBJECT

public:
    /// Qt special value that can be passed in to setResizeContentsPrecision()
    /// that means to consider only visible rows when resizing columns to fit
    /// cell contents.
    static const int kResizeContentsPrecision_VisibleRows = 0;

    /// Qt special value that can be passed in to setResizeContentsPrecision()
    /// that means to consider all rows when resizing columns to fit cell
    /// contents.
    static const int kResizeContentsPrecision_AllRows = -1;

    /// Constructor
    /// \param orientation Either Qt::Horizontal or Qt::Vertical
    /// \param parent the parent widget
    ScaledHeaderView(Qt::Orientation orientation, QWidget* parent = nullptr);

    /// Virtual destructor
    virtual ~ScaledHeaderView();

    /// Set additional padding after the text to space out the columns.
    /// \param padding Padding width in pixels
    void SetColumnPadding(const int padding)
    {
        column_padding_ = padding;
    }

    /// Sets the column width to the specified width in em's and will
    /// change the column ResizeMode to Fixed.
    /// Setting column width based on em's allows the column to be properly
    /// resize on DPI scale changes. See AutoResizeColumns().
    void SetColumnWidthEms(const int column, const int em_count);

    /// Automatically resizes column widths based on their em-count.
    /// Columns that do not have an em-count will not be resized, but are
    /// probably already at an expected width.
    void AutoResizeColumns();

protected:
    /// Overridden sizeHint that accommodates for scaled font sizes.
    /// \param logical_index the index of the section to get the size from contents.
    virtual QSize sectionSizeFromContents(int logical_index) const Q_DECL_OVERRIDE;

private:
    /// Calculate addtional space needed for the sort indicator if it is being shown.
    /// \return The additional space needed for the sort indicator. Note that this should
    /// always be 0 in one dimension, because the sort indicator will either be above/below
    /// the text (ie: additional height needed), or to the left/right of the text (additional
    /// width needed). It may also return (0,0) if the sort indicator is not being used on the
    /// header.
    QSize GetSpaceForSortIndicator() const;

    /// Calculates a section size based on the previously specified em-count.
    /// \param column The column for which to calculate a section size.
    /// \return A desired section size for the specified column based on the current
    /// font and em-count. If the supplied column did not have an em-count specified,
    /// then an invalid QSize is returned.
    QSize SectionSizeFromEmCount(const int column) const;

    /// Allows the use of em's to define the column widths.
    /// em's are the width of the letter 'm', which allows the column widths
    /// to rescale based on DPI scale factor without having to remeasure cell contents.
    /// This vector will contain a unique em-count for each column of the table.
    QVector<int> column_width_em_counts_;

    /// Additional padding in pixels after the text to space out the columns.
    int column_padding_;
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SCALED_HEADER_VIEW_H_
