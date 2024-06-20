//=============================================================================
/// Copyright (c) 2016-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation for a ruler.
//=============================================================================

#include "ruler_widget.h"

#include <math.h> /* log2 */
#include <QtWidgets>

#include "qt_util.h"
#include "scaling_manager.h"

// min and max tick step. This is the number of pixels between each tick or
// vertical line on the ruler. Currently assumes a value between 5 and 10 pixels
// is good.
static const double kMinimumTickStep = 5.0;
static const double kMaximumTickStep = 10.0;

static const float kRulerFontPointSize = 8.25f;

void RulerWidget::PaintRulerBackground(QPainter* painter, const QRectF& rect)
{
    painter->save();

    auto pen   = painter->pen();
    auto brush = painter->brush();

    pen.setColor(QtCommon::QtUtils::ColorTheme::Get().GetCurrentThemeColors().ruler_edge_color);
    brush.setColor(QtCommon::QtUtils::ColorTheme::Get().GetCurrentThemeColors().ruler_background_color);

    pen.setCosmetic(true); // Don't scale.

    painter->setPen(pen);
    painter->setBrush(brush);

    painter->drawRect(rect);

    painter->restore();
}

double RulerWidget::GetStartingTime(double min_visible_time, uint64_t time_period, bool use_time_period)
{
    if (use_time_period == false)
    {
        return min_visible_time;
    }
    else
    {
        return (min_visible_time - time_period) < 0 ? 0 : (min_visible_time - time_period);
    }
}

void RulerWidget::PaintRuler(QPainter*     painter,
                             const QRectF& rect,
                             uint64_t      max_time,
                             uint64_t      min_visible_clk,
                             uint64_t      max_visible_clk,
                             double        time_to_clk_ratio,
                             int           unit_type,
                             bool          use_time_period)
{
    painter->save();

    auto font = painter->font();
    font.setPointSizeF(kRulerFontPointSize);
    painter->setFont(font);

    auto pen = painter->pen();

    pen.setColor(QtCommon::QtUtils::ColorTheme::Get().GetCurrentThemeColors().ruler_marker_color);
    pen.setCosmetic(true); // Don't scale.

    painter->setPen(pen);

    // convert maxtime, minvisible, maxvisible to nanoseconds if time units not clock
    // This is so the ruler scaling can be done using the time units specified and
    // not just clocks.
    double max_clock_time   = time_to_clk_ratio * (double)max_time;
    double max_visible_time = time_to_clk_ratio * (double)max_visible_clk;
    double min_visible_time = time_to_clk_ratio * (double)min_visible_clk;

    // increment can go from 5 to 10
    const int32_t markers_per_section = 20;
    const int     marker_unit_height  = rect.height() / 8;

    // time period is the time between the numeric values displayed on the ruler ie
    // if the ruler displayed 10000, 20000, 30000, the time period is 10000
    uint64_t time_period = 1;

    double ruler_length = rect.width();
    //Q_ASSERT(ruler_length > 0);
    double num_sections = (markers_per_section * max_clock_time);

    // calculate how big the tick step is for the whole ruler. This will be some
    // arbitrary value
    double tick_step = ruler_length / num_sections;

    // If the ticks are too close together, keep multiplying by 10. This will force the ticks
    // further apart and will also increase the number displayed in each tick. For example, using
    // values of 0, 10, 20, 30 .. may cause the ruler to be too squashed, so maybe having ruler
    // segments of 0, 100, 200 (more spaced out) may be better
    const double lower_limit = kMinimumTickStep;
    while (tick_step < lower_limit)
    {
        // ie 10, 100, 1000, 10000 etc.
        time_period *= 10;
        num_sections = (markers_per_section * max_clock_time) / time_period;
        tick_step    = ruler_length / num_sections;
    }

    // Scale the ticks back down in the case where the gap between them is too large. Up to this
    // part, the scaling will be in base 10 but maybe the gap between 10000 and 20000 is too large.
    // In this case, the tick_step is halved so this goes down to 10000, 15000, 20000.
    const double upper_limit = kMaximumTickStep;
    while (tick_step > upper_limit)
    {
        time_period /= 2;
        //Q_ASSERT(time_period > 0);
        num_sections = (markers_per_section * max_clock_time) / time_period;
        tick_step    = ruler_length / num_sections;
    }

    uint64_t count       = 0;
    double   clock_label = 0;

    double scale_increment = time_period;
    scale_increment /= ((double)markers_per_section);

    // quick calculation to start the ruler at the start of the draw region
    const double starting_time = GetStartingTime(min_visible_time, time_period, use_time_period);
    count                      = starting_time / scale_increment;
    clock_label                = scale_increment * count;

    double x_pos = rect.left() + (count * tick_step);

    // make sure the scale is valid before proceeding.
    if (scale_increment > 0)
    {
        while (clock_label <= (max_visible_time + scale_increment))
        {
            const bool perform_draw = (clock_label >= starting_time);
            int        top;

            // long marker
            if ((count % markers_per_section) == 0)
            {
                top = rect.bottom() - (marker_unit_height * 4);

                // draw the text if visible
                if (perform_draw == true)
                {
                    // Use the world transform to translate to the place we want the text to be,
                    // render the text, then reset the translation.
                    QTransform wt = painter->worldTransform();
                    wt.translate(x_pos, 0);
                    painter->setWorldTransform(wt);
                    painter->drawText(1, top - (marker_unit_height * 2), QtCommon::QtUtils::ClockToTimeUnit(clock_label, unit_type));
                    wt.translate(-x_pos, 0);
                    painter->setWorldTransform(wt);
                }
            }

            // mid marker
            else if ((count % 10) == 0)
            {
                top = rect.bottom() - (marker_unit_height * 3);
            }

            // short marker
            else
            {
                top = rect.bottom() - (marker_unit_height * 2);
            }

            if (perform_draw == true)
            {
                QTransform wt = painter->worldTransform();
                wt.translate(x_pos, 0);
                painter->setWorldTransform(wt);
                painter->drawLine(1, top, 1, rect.bottom() - 1);
                wt.translate(-x_pos, 0);
                painter->setWorldTransform(wt);
            }

            clock_label += scale_increment;
            count++;
            x_pos += tick_step;
        }
    }
    painter->restore();
}

RulerWidget::RulerWidget(const RulerConfig& config)
    : minimum_visible_clock_(0)
    , maximum_visible_clock_(0)
{
    Update(config);

    // Ruler always on top
    setZValue(1);
}

QRectF RulerWidget::boundingRect() const
{
    return QRectF(0, 0, config_.width, config_.height);
}

QPainterPath RulerWidget::shape() const
{
    QPainterPath path;
    path.addRect(0, 0, config_.width, config_.height);
    return path;
}

void RulerWidget::Update(const RulerConfig& config)
{
    memcpy(&config_, &config, sizeof(config_));
}

void RulerWidget::UpdateVisibleBounds(uint64_t min_clk, uint64_t max_clk)
{
    minimum_visible_clock_ = min_clk;
    maximum_visible_clock_ = max_clk;
}

void RulerWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // only draw the ruler if there's a time difference, otherwise there's nothing to show
    if (maximum_visible_clock_ > minimum_visible_clock_)
    {
        PaintRulerBackground(painter, boundingRect());
        PaintRuler(painter, boundingRect(), config_.max_time, minimum_visible_clock_, maximum_visible_clock_, config_.time_to_clock_ratio, config_.unit_type);
    }
}
