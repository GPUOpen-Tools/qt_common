//=============================================================================
/// Copyright (c) 2019-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a scaled QHeaderView
//=============================================================================

#include "scaled_header_view.h"

#include <QAbstractProxyModel>
#include <QEvent>
#include <QStandardItemModel>

#include "scaling_manager.h"

ScaledHeaderView::ScaledHeaderView(Qt::Orientation orientation, QWidget* parent)
    : QHeaderView(orientation, parent)
    , column_padding_(0)
{
}

ScaledHeaderView::~ScaledHeaderView()
{
}

QSize ScaledHeaderView::sectionSizeFromContents(int logical_index) const
{
    ensurePolished();

    QSize size_hint;

    QAbstractItemModel* item_model = model();
    if (item_model != nullptr)
    {
        if (orientation() == Qt::Horizontal)
        {
            // Get size hint from em count
            size_hint = SectionSizeFromEmCount(logical_index);
        }

        if (!size_hint.isValid())
        {
            // Get default size hint
            size_hint = QHeaderView::sectionSizeFromContents(logical_index);
        }
    }

    // Add column padding to the width.
    size_hint.setWidth(size_hint.width() + column_padding_);

    return size_hint;
}

void ScaledHeaderView::SetColumnWidthEms(const int column, const int em_count)
{
    if (column >= 0)
    {
        // resize list if needed
        if (column >= column_width_em_counts_.size())
        {
            column_width_em_counts_.resize(column + 1);
        }

        column_width_em_counts_[column] = em_count;

        // Set resize mode to fixed so that the column widths are programmatically resizeable,
        // but are not user-resizable (which mimics the default ResizeToContents behavior).
        // This still allows the application to override the ResizeMode to Interactive if desired.
        setSectionResizeMode(column, QHeaderView::ResizeMode::Fixed);
    }
}

QSize ScaledHeaderView::SectionSizeFromEmCount(const int column) const
{
    ensurePolished();

    QSize section_size;

    if (column >= 0 && column < column_width_em_counts_.size())
    {
        int em_count = column_width_em_counts_[column];
        if (em_count > 0)
        {
            section_size.setHeight(QHeaderView::sectionSizeFromContents(column).height());
            int em_width = fontMetrics().boundingRect('M').width();
            section_size.setWidth(em_count * em_width);

            // Make additional adjustments if there is a sort indicator on this section.
            QSize sort_size = GetSpaceForSortIndicator();
            section_size += sort_size;
        }
    }

    return section_size;
}

void ScaledHeaderView::AutoResizeColumns()
{
    for (int i = 0; i < count(); ++i)
    {
        QSize section_size = SectionSizeFromEmCount(i);
        if (section_size.isValid())
        {
            // Store original resize mode
            QHeaderView::ResizeMode original_resize_mode = sectionResizeMode(i);

            // Make sure the section will respond to resizing,
            // and resize the section.
            setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
            resizeSection(i, section_size.width());

            // Restore original resize mode
            setSectionResizeMode(original_resize_mode);
        }
    }
}

QSize ScaledHeaderView::GetSpaceForSortIndicator() const
{
    QSize   space(0, 0);
    QStyle* style = this->style();

    if (style != nullptr && isSortIndicatorShown())
    {
        int margin    = style->pixelMetric(QStyle::PM_HeaderMargin, nullptr, this);
        int mark_size = style->pixelMetric(QStyle::PM_HeaderMarkSize, nullptr, this);

        // The style (and other options) controls whether the sort indicator is above / below,
        // or the left / right of the text. Determine where the sort indicator will be
        // and add the extra necessary space to that dimension. This ensures that the header has
        // enough space to draw the sort indicator.
        Qt::Alignment alignment = QFlags<Qt::AlignmentFlag>(style->styleHint(QStyle::StyleHint::SH_Header_ArrowAlignment, nullptr, this));
        if ((alignment & Qt::AlignTop) || (alignment & Qt::AlignBottom))
        {
            space.setWidth(0);
            space.setHeight(margin + mark_size);
        }
        else
        {
            space.setWidth(margin + mark_size);
            space.setHeight(0);
        }
    }

    return space;
}
