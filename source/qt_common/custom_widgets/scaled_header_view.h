//=============================================================================
/// Copyright (c) 2019-2025 Advanced Micro Devices, Inc. All rights reserved.
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

    /// @brief Sets the column width to the specified width in em's and changes the column ResizeMode to Fixed.
    ///
    /// @param [in] column   The column index to set a fixed width for.
    /// @param [in] em_count The em count to use to calculate a fixed width.
    ///
    /// @deprecated Use of this function is strongly discouraged now that Qt 6 supports high dpi scaling.
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
    /// Calculates a section size based on the previously specified em-count.
    /// \param column The column for which to calculate a section size.
    /// \return A desired section size for the specified column based on the current
    /// font and em-count. If the supplied column did not have an em-count specified,
    /// then an invalid QSize is returned.
    QSize SectionSizeFromEmCount(const int column) const;

    /// @brief Calculate addtional space needed for the sort indicator if it is being shown.
    ///
    /// @return The additional space needed for the sort indicator.
    QSize GetSpaceForSortIndicator() const;

    /// Allows the use of em's to define the column widths.
    /// em's are the width of the letter 'm', which allows the column widths
    /// to rescale based on DPI scale factor without having to remeasure cell contents.
    /// This vector will contain a unique em-count for each column of the table.
    QVector<int> column_width_em_counts_;

    /// Additional padding in pixels after the text to space out the columns.
    int column_padding_;
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SCALED_HEADER_VIEW_H_
