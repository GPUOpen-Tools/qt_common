//=============================================================================
// Copyright (c) 2023 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Shared isa tree vertical scroll bar implementation.
//=============================================================================

#include "shared_isa_vertical_scroll_bar.h"

#include <QPainter>
#include <QStyleOptionSlider>

#include "shared_isa_tree_view.h"

#include "qt_util.h"

SharedIsaVerticalScrollBar::SharedIsaVerticalScrollBar(QWidget* parent)
    : QScrollBar(parent)
    , hot_spot_line_number_(-1)
{
}

SharedIsaVerticalScrollBar::~SharedIsaVerticalScrollBar()
{
}

void SharedIsaVerticalScrollBar::SetHotSpotLineNumber(int line_number)
{
    hot_spot_line_number_ = line_number;

    update();
}

void SharedIsaVerticalScrollBar::SetSearchMatchLineNumbers(const std::set<int>& line_numbers)
{
    search_match_line_numbers_ = line_numbers;

    update();
}

void SharedIsaVerticalScrollBar::paintEvent(QPaintEvent* event)
{
    // Let Qt paint entire scrollbar first.
    QScrollBar::paintEvent(event);

    if (search_match_line_numbers_.empty() && hot_spot_line_number_ == -1)
    {
        return;
    }

    QStyleOptionSlider option;
    this->initStyleOption(&option);

    // Get the pixel height of the up arrow button in this scroll bar.
    const qreal button_pixel_height =
        style()->subControlRect(QStyle::CC_ScrollBar, qstyleoption_cast<QStyleOptionComplex*>(&option), QStyle::SC_ScrollBarSubLine).height();

    // Get the pixel height of the scrollable space in this scroll bar.
    qreal scroll_bar_pixel_height = style()->pixelMetric(QStyle::PM_SliderSpaceAvailable, qstyleoption_cast<QStyleOption*>(&option));
    scroll_bar_pixel_height       = scroll_bar_pixel_height - (2 * button_pixel_height);

    // Get the # of lines of isa.
    const qreal number_lines = maximum() - minimum() + pageStep();

    // Get the pixel height of a single row mapped to the inside of the scroll bar; round up to 1 pixel.
    qreal row_in_scroll_bar_pixel_height = (scroll_bar_pixel_height * 1) / number_lines;
    row_in_scroll_bar_pixel_height       = (row_in_scroll_bar_pixel_height < 1.0) ? 1.0 : row_in_scroll_bar_pixel_height;

    // Turn on AA and paint using floats to try and help improve accuracy.
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Get the width and height of the rectangle(s) we want to paint inside this scroll bar.
    const qreal scroll_bar_half_width = option.rect.width() / 2;
    QRectF      scroll_bar_rectangle  = option.rect;
    scroll_bar_rectangle.adjust(1, 0, -1, 0);
    scroll_bar_rectangle.setHeight(row_in_scroll_bar_pixel_height);

    if (!search_match_line_numbers_.empty())
    {
        for (const auto& search_match_line_number : search_match_line_numbers_)
        {
            // Normalize the y pixel position of the search match line number(s).
            const qreal qreal_search_match_line_number = search_match_line_number;
            const qreal search_match_y_pixel_position  = ((scroll_bar_pixel_height * qreal_search_match_line_number) / number_lines) + button_pixel_height;

            // Paint the search match rectangle on top and to the right.
            QRectF search_match_rectangle = scroll_bar_rectangle;
            search_match_rectangle.adjust(scroll_bar_half_width, search_match_y_pixel_position, 0, search_match_y_pixel_position);
            QColor search_match_row_color = QtCommon::QtUtils::ColorTheme::Get().GetCurrentThemeColors().isa_search_match_row_color;
            painter.fillRect(search_match_rectangle, search_match_row_color);
        }
    }

    if (hot_spot_line_number_ != -1)
    {
        // Normalize the y pixel position of the hot spot line number.
        const qreal qreal_hot_spot_line_number = hot_spot_line_number_;
        const qreal hot_spot_y_pixel_position  = ((scroll_bar_pixel_height * qreal_hot_spot_line_number) / number_lines) + button_pixel_height;

        // Paint hot spot rectangle on top and to the left.
        QRectF hot_spot_rectangle = scroll_bar_rectangle;
        hot_spot_rectangle.adjust(0, hot_spot_y_pixel_position, -scroll_bar_half_width, hot_spot_y_pixel_position);
        painter.fillRect(hot_spot_rectangle, Qt::red);
    }
}