//=============================================================================
/// Copyright (c) 2016-2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a donut pie graphics item widget.
//=============================================================================

// use definition of PI from math.h
#define _USE_MATH_DEFINES

#include "DonutPieWidget.h"
#include <math.h>
#include <QQueue>
#include <QPainter>
#include <../Util/QtUtil.h>

static const int s_DONUT_VALUE_PIXEL_FONT_SIZE = 36;
static const int s_DONUT_TEXT_PIXEL_FONT_SIZE = 14;

//-----------------------------------------------------------------------------
/// Explicit constructor
/// \param width The widget's width
/// \param height The widget's height
//-----------------------------------------------------------------------------
DonutPieWidget::DonutPieWidget(int width, int height) :
    m_width(width),
    m_height(height),
    m_numSegments(0),
    m_arcWidth(1.0),
    m_textLineOne(""),
    m_textLineTwo(""),
    m_backgroundColor(Qt::white)
{
}

//-----------------------------------------------------------------------------
/// Implementation of Qt's bounding volume for this item.
/// \return The item's bounding rectangle.
//-----------------------------------------------------------------------------
QRectF DonutPieWidget::boundingRect() const
{
    return QRectF(0, 0, m_width, m_height);
}

//-----------------------------------------------------------------------------
/// Implementation of Qt's paint for this item.
/// \param pPainter The painter object to use.
/// \param pOption Provides style options for the item, such as its state, exposed area and its level-of-detail hints.
/// \param pWidget Points to the widget that is being painted on if specified.
//-----------------------------------------------------------------------------
void DonutPieWidget::paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget)
{
    Q_UNUSED(pOption);
    Q_UNUSED(pWidget);

    QRectF boundRect = boundingRect();

    int w = boundRect.width();
    int h = boundRect.height();

    pPainter->setRenderHint(QPainter::Antialiasing);
    pPainter->fillRect(boundRect, m_backgroundColor);

    // calculate the total range of the values. This is used to calculate how wide
    // each segment should be
    qreal range = 0.0;

    // TODO: use an iterator and only calculate range if needed
    for (unsigned int loop = 0; loop < m_numSegments; loop++)
    {
        range += m_slices[loop].m_value;
    }

    // calculate the draw rectangle. Take into account the width of the pen and subtract this
    // from the rectangle bounds
    QRectF rect(m_arcWidth / 2.0, m_arcWidth / 2.0, w - m_arcWidth, h - m_arcWidth);

    QFont font;
    font.setFamily(font.defaultFamily());
    // iterate over all segments and draw each one
    // set start to 6 o'clock position, clockwise (default is 3 o'clock, so add 90 degrees counterclockwise)
    // angles are specified in 1/16 of a degree, negative angles are counterclockwise
    int startPos = -90 * 16;

    QQueue<QPoint> labelPositions;
    for (unsigned int loop = 0; loop < m_numSegments; loop++)
    {
        // create the pen and set up the color for this slice
        QPen fgPen(m_slices[loop].m_fillColor, m_arcWidth, Qt::SolidLine);
        fgPen.setCapStyle(Qt::FlatCap);

        // calculate the arc angle for this slice
        int angle = (360 * 16 * m_slices[loop].m_value) / range;

        // draw the arc
        pPainter->setPen(fgPen);
        pPainter->drawArc(rect, startPos, angle);

        // figure out where to draw the text on the arc
        qreal textAngle = angle / 2;
        textAngle += startPos;

        // convert to radians
        textAngle *= M_PI / (180 * 16);

        // calculate text position
        int radius = rect.width() / 2;
        qreal xPos = radius + (radius * cos(textAngle));
        qreal yPos = radius - (radius * sin(textAngle));

        // take into account the donut draw rectangle and the bounding rectangle of the font
        QRect textRect = pPainter->boundingRect(QRect(0, 0, 0, 0), Qt::AlignLeft, m_slices[loop].m_sliceText);
        xPos += rect.x() - (textRect.width() / 2);
        yPos += rect.y() + (textRect.height() / 2);

        // Save label positions and render later once all arc sections have been drawn.
        labelPositions.enqueue(QPoint(xPos, yPos));

        // set the start position of the next arc
        startPos += angle;
    }

    // draw the text labels on the arcs
    pPainter->setPen(Qt::white);
    for (unsigned int loop = 0; loop < m_numSegments; loop++)
    {
        const QPoint labelPos = labelPositions.dequeue();
        pPainter->drawText(labelPos.x(), labelPos.y(), m_slices[loop].m_sliceText);
    }

    font.setPixelSize(s_DONUT_VALUE_PIXEL_FONT_SIZE);
    pPainter->setFont(font);
    pPainter->setPen(Qt::black);

    int textWidth = QtCommon::QtUtil::GetPainterTextWidth(pPainter, m_textLineOne);

    int xPos = (w - textWidth) / 2;
    int yPos = (h * 52) / 100;
    pPainter->drawText(xPos, yPos, m_textLineOne);

    // draw the description text
    font.setPixelSize(s_DONUT_TEXT_PIXEL_FONT_SIZE);
    pPainter->setFont(font);
    textWidth = QtCommon::QtUtil::GetPainterTextWidth(pPainter, m_textLineTwo);
    xPos = (w - textWidth) / 2;
    pPainter->drawText(xPos, (h * 66) / 100, m_textLineTwo);
}

//-----------------------------------------------------------------------------
/// Set the number of segments for this control. This is the number of
/// unique data elements to be shown in this control
/// \param numSegments the number of segments needed
//-----------------------------------------------------------------------------
void DonutPieWidget::setNumSegments(unsigned int numSegments)
{
    if (m_numSegments != numSegments)
    {
        m_slices.resize(numSegments);
        m_numSegments = numSegments;
    }
}

//-----------------------------------------------------------------------------
/// Set the value for the given index for the control.
/// \param index the index whose value is to change
/// \param value the new value to use
//-----------------------------------------------------------------------------
void DonutPieWidget::setIndexValue(unsigned int index, qreal value)
{
    if (index < m_numSegments)
    {
        m_slices[index].m_value = value;
    }
}

//-----------------------------------------------------------------------------
/// Set the fill color for the given index for the control.
/// \param index the index whose color is to change
/// \param fillColor the color to use
//-----------------------------------------------------------------------------
void DonutPieWidget::setIndexColor(unsigned int index, const QColor& fillColor)
{
    if (index < m_numSegments)
    {
        m_slices[index].m_fillColor = fillColor;
    }
}

//-----------------------------------------------------------------------------
/// Set the text to be displayed in the pie segment.
/// \param index the index whose text is to change
/// \param text the text to be shown
//-----------------------------------------------------------------------------
void DonutPieWidget::setIndexText(unsigned int index, const QString& text)
{
    if (index < m_numSegments)
    {
        m_slices[index].m_sliceText = text;
    }
}

//-----------------------------------------------------------------------------
/// Set how wide the donut section should be.
/// \param arcWidth the width of the donut arc
//-----------------------------------------------------------------------------
void DonutPieWidget::setArcWidth(qreal arcWidth)
{
    m_arcWidth = arcWidth;
}

//-----------------------------------------------------------------------------
/// Set the first line of text inside the donut
/// \param text Text to set
//-----------------------------------------------------------------------------
void DonutPieWidget::setTextLineOne(const QString& text)
{
    m_textLineOne = text;
}

//-----------------------------------------------------------------------------
/// Set the second line of text inside the donut
/// \param text Text to set
//-----------------------------------------------------------------------------
void DonutPieWidget::setTextLineTwo(const QString& text)
{
    m_textLineTwo = text;
}

//-----------------------------------------------------------------------------
/// Set the background color
/// \param color The color to set
//-----------------------------------------------------------------------------
void DonutPieWidget::setBackgroundColor(const QColor& color)
{
    m_backgroundColor = color;
}
