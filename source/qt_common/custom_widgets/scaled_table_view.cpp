//=============================================================================
/// Copyright (c) 2020-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a scaled QTableView
//=============================================================================

#include "scaled_table_view.h"

#include <QEvent>
#include <QScrollBar>

#include "common_definitions.h"
#include "qt_util.h"
#include "scaling_manager.h"

/// Specifies the default number of rows to measure when calculating
/// column width if the HeaderView's ResizeMode is set to ResizeToContents.
/// A higher values results in a potentially more accurate column width, but
/// increases the performance penalty due to measuring more cell contents.
/// The Qt default is 100; special value of -1 means to measure every cell;
/// special value of 0 means to measure only visible cells.
/// The setting is ignored for other ResizeModes.
/// To avoid the performance penalty and get a similar resizing behavior,
/// consider whether it is possible to use a Fixed or Interactive ResizeMode
/// and set the column widths using our custom SetColumnWidthEms() method.
const int kDefaultRowsToCheckForColumnWidth = 32;

ScaledTableView::ScaledTableView(QWidget* parent)
    : QTableView(parent)
    , column_padding_(kScaledTableViewDefaultColumnPadding)
    , horizontal_header_(nullptr)
    , vertical_header_(nullptr)
{
    // The QTableView will take ownership of the headers and delete it when needed.
    horizontal_header_ = new ScaledHeaderView(Qt::Horizontal, this);
    this->setHorizontalHeader(horizontal_header_);
    vertical_header_ = new ScaledHeaderView(Qt::Vertical, this);
    this->setVerticalHeader(vertical_header_);

    SetDefaultProperties(kScaledTableViewDefaultColumnPadding);

    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledTableView::OnScaleFactorChanged);
}

ScaledTableView::~ScaledTableView()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledTableView::OnScaleFactorChanged);
}

void ScaledTableView::SetDefaultProperties(int padding)
{
    // The headers should have been set during the constructor,
    // so confirm that is true.
    Q_ASSERT(horizontal_header_ != nullptr);
    Q_ASSERT(vertical_header_ != nullptr);
    horizontal_header_->setDefaultAlignment(Qt::AlignLeft);
    horizontal_header_->setSectionsClickable(false);
    horizontal_header_->setResizeContentsPrecision(kDefaultRowsToCheckForColumnWidth);
    horizontal_header_->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    horizontal_header_->setStretchLastSection(true);

    vertical_header_->setSectionsClickable(false);
    vertical_header_->setVisible(false);
    vertical_header_->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);

    SetColumnPadding(padding);

    setFrameStyle(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    verticalScrollBar()->blockSignals(false);
    horizontalScrollBar()->blockSignals(false);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setShowGrid(false);
    setWordWrap(false);
    setAlternatingRowColors(true);
}

ScaledHeaderView* ScaledTableView::GetHeaderView()
{
    return horizontal_header_;
}

void ScaledTableView::SetColumnPadding(int padding)
{
    column_padding_ = padding;

    if (horizontal_header_ != nullptr)
    {
        horizontal_header_->SetColumnPadding(padding);
    }

    if (vertical_header_ != nullptr)
    {
        vertical_header_->SetColumnPadding(padding);
    }
}

int ScaledTableView::sizeHintForColumn(int column) const
{
    int width = QTableView::sizeHintForColumn(column);

    width += column_padding_;

    return width;
}

void ScaledTableView::OnScaleFactorChanged()
{
    ensurePolished();

    // Invalidate cached fontMetrics.
    QtCommon::QtUtils::InvalidateFontMetrics(this);
    QtCommon::QtUtils::InvalidateFontMetrics(horizontal_header_);
    QtCommon::QtUtils::InvalidateFontMetrics(vertical_header_);

    horizontal_header_->AutoResizeColumns();

    // Update geometry of the QTableView since it may need more or less space now.
    updateGeometry();

    // Update the contents being displayed in the view.
    update();
}

void ScaledTableView::SetColumnWidthEms(const int column, const int em_count)
{
    horizontal_header_->SetColumnWidthEms(column, em_count);
}
