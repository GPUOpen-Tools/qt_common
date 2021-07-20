//=============================================================================
/// Copyright (c) 2016-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a donut pie graphics item widget.
//=============================================================================

// use definition of PI from math.h
#define _USE_MATH_DEFINES

#include "donut_pie_widget.h"

#include <math.h>
#include <QPainter>
#include <QQueue>

#include "utils/qt_util.h"
#include "utils/scaling_manager.h"

DonutPieWidget::DonutPieWidget(QWidget* parent)
    : QWidget(parent)
    , num_segments_(0)
    , arc_width_(1.0)
    , text_line_one_("")
    , text_line_two_("")
{
    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &QWidget::updateGeometry);
}

DonutPieWidget::~DonutPieWidget()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &QWidget::updateGeometry);
}

QSize DonutPieWidget::sizeHint() const
{
    return ScalingManager::Get().Scaled(QSize(kDefaultWidthAndHeight_, kDefaultWidthAndHeight_));
}

void DonutPieWidget::paintEvent(QPaintEvent* paint_event)
{
    Q_UNUSED(paint_event);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), palette().window());

    const int width  = rect().width();
    const int height = rect().height();

    const qreal scaled_arc_width = ScalingManager::Get().Scaled(arc_width_);

    // Determine widest label.
    int                 max_width    = 0;
    const QFontMetrics& font_metrics = ScalingManager::Get().ScaledFontMetrics(font());
    for (unsigned int index = 0; index < num_segments_; ++index)
    {
        const int text_width = font_metrics.boundingRect(slices_[index].slice_text_).width();

        max_width = std::max(max_width, text_width);
    }

    // Add half the width of the widest label as a buffer around the widget.
    // subtract the width of the arc since the arc runs through the text.
    // Do not allow a buffer that is negative.
    qreal arc_buffer = std::max(0.0, (max_width - scaled_arc_width) / 2.0);

    // calculate the total range of the values. This is used to calculate how wide
    // each segment should be
    qreal range = 0.0;

    for (unsigned int loop = 0; loop < num_segments_; loop++)
    {
        range += slices_[loop].value_;
    }

    // calculate the draw rectangle. Take into account the width of the pen and subtract this
    // from the rectangle bounds
    QRectF arc_rect((scaled_arc_width / 2.0) + arc_buffer,
                    (scaled_arc_width / 2.0) + arc_buffer,
                    width - scaled_arc_width - (2 * arc_buffer),
                    height - scaled_arc_width - (2 * arc_buffer));

    const int radius = (arc_rect.width() / 2);

    QFont font;
    font.setFamily(font.defaultFamily());
    // iterate over all segments and draw each one
    // set start to 6 o'clock position, clockwise (default is 3 o'clock, so add 90 degrees counterclockwise)
    // angles are specified in 1/16 of a degree, negative angles are counterclockwise
    int start_pos = -90 * 16;

    QQueue<QPoint> label_positions;
    for (unsigned int loop = 0; loop < num_segments_; loop++)
    {
        // create the pen and set up the color for this slice
        QPen foreground_pen(slices_[loop].fill_color_, scaled_arc_width, Qt::SolidLine);
        foreground_pen.setCapStyle(Qt::FlatCap);

        // calculate the arc angle for this slice
        int angle = (360 * 16 * slices_[loop].value_) / range;

        // draw the arc
        painter.setPen(foreground_pen);
        painter.drawArc(arc_rect, start_pos, angle);

        // figure out where to draw the text on the arc
        qreal text_angle = angle / 2;
        text_angle += start_pos;

        // convert to radians
        text_angle *= M_PI / (180 * 16);

        // calculate text position
        const QPointF center_point = arc_rect.center();

        qreal x_pos = center_point.x() + (radius * cos(text_angle));
        qreal y_pos = center_point.y() - (radius * sin(text_angle));

        // take into account the donut draw rectangle and the bounding rectangle of the font
        QRect textRect = painter.boundingRect(QRect(0, 0, 0, 0), Qt::AlignLeft, slices_[loop].slice_text_);
        x_pos -= (textRect.width() / 2);
        y_pos += (textRect.height() / 2);

        // Save label positions and render later once all arc sections have been drawn.
        label_positions.enqueue(QPoint(x_pos, y_pos));

        // set the start position of the next arc
        start_pos += angle;
    }

    // draw the text labels on the arcs
    painter.setPen(palette().windowText().color());
    for (unsigned int loop = 0; loop < num_segments_; loop++)
    {
        const QPoint label_pos = label_positions.dequeue();
        painter.drawText(label_pos.x(), label_pos.y(), slices_[loop].slice_text_);
    }

    // Draw the description text
    font.setPixelSize(ScalingManager::Get().Scaled(kValuePixelFontSize_));
    painter.setFont(font);
    painter.setPen(Qt::black);

    int text_width = QtCommon::QtUtils::GetPainterTextWidth(&painter, text_line_one_);

    int desc_x_pos = (width - text_width) / 2;
    int desc_y_pos = (height * 52) / 100;
    painter.drawText(desc_x_pos, desc_y_pos, text_line_one_);

    font.setPixelSize(ScalingManager::Get().Scaled(kTextPixelFontSize_));
    painter.setFont(font);
    text_width = QtCommon::QtUtils::GetPainterTextWidth(&painter, text_line_two_);
    desc_x_pos = (width - text_width) / 2;
    painter.drawText(desc_x_pos, (height * 66) / 100, text_line_two_);
}

void DonutPieWidget::SetNumSegments(unsigned int numSegments)
{
    if (num_segments_ != numSegments)
    {
        slices_.resize(numSegments);
        num_segments_ = numSegments;
    }
}

void DonutPieWidget::SetIndexValue(unsigned int index, qreal value)
{
    if (index < num_segments_)
    {
        slices_[index].value_ = value;
    }
}

void DonutPieWidget::SetIndexColor(unsigned int index, const QColor& fillColor)
{
    if (index < num_segments_)
    {
        slices_[index].fill_color_ = fillColor;
    }
}

void DonutPieWidget::SetIndexText(unsigned int index, const QString& text)
{
    if (index < num_segments_)
    {
        slices_[index].slice_text_ = text;
    }

    updateGeometry();
}

void DonutPieWidget::SetArcWidth(qreal arcWidth)
{
    arc_width_ = arcWidth;
}

void DonutPieWidget::SetTextLineOne(const QString& text)
{
    text_line_one_ = text;
}

void DonutPieWidget::SetTextLineTwo(const QString& text)
{
    text_line_two_ = text;
}
