//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of up/down arrow widget.
//=============================================================================

#include <QPainter>
#include <ArrowIconWidget.h>
#include <../Scaling/ScalingManager.h>
#include <../Util/CommonDefinitions.h>

static const int s_TEXT_OFFSET_X = 4;
static const int s_TEXT_OFFSET_Y = 5;
static const int s_CENTER_UP_ARROW = 5;
static const int s_CENTER_DOWN_ARROW = 9;
static const int s_BUTTON_BASE_SIZE = 18;
static const int s_PEN_WIDTH = 3;

//-----------------------------------------------------------------------------
/// Explicit constructor.
/// \param pParent The parent widget
//-----------------------------------------------------------------------------
ArrowIconWidget::ArrowIconWidget(QWidget* pParent) :
    QPushButton(pParent)
{
    // Set default values
    m_size = s_BUTTON_BASE_SIZE;
    m_direction = Direction::DownArrow;
    m_color = Qt::GlobalColor::gray;
    m_penWidth = s_PEN_WIDTH;

    // Create the vertices
    CreateVertices();
}

//-----------------------------------------------------------------------------
/// Overridden paintEvent method.
/// \param event The paint event object
//-----------------------------------------------------------------------------
void ArrowIconWidget::paintEvent(QPaintEvent* pEvent)
{
    Q_UNUSED(pEvent);

    // Set up the painter
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.save();

    // Get the scaling factor
    const double scalingFactor = ScalingManager::Get().GetScaleFactor();

    // Set properties for the lines
    QPen pen;
    pen.setColor(m_color);
    pen.setWidth(m_penWidth * scalingFactor);
    painter.setPen(pen);

    // Calculate new points using the current scale factor
    QPointF scaledPoints[s_NUMBER_OF_VERTICES];
    for (int i = 0; i < s_NUMBER_OF_VERTICES; i++)
    {
        scaledPoints[i].setX(m_vertices[i].x() * scalingFactor);
        scaledPoints[i].setY(m_vertices[i].y() * scalingFactor);
    }

    // Position the paint object
    painter.translate(0, -s_CENTER_UP_ARROW * scalingFactor);

    // Rotate the paint object if generating an up arrow
    if (m_direction == Direction::UpArrow)
    {
        painter.rotate(180);
        // Also translate the paint object towards the bottom of the rect
        painter.translate(0, 0);
        painter.translate(-m_size * scalingFactor, (-m_size - s_CENTER_DOWN_ARROW) * scalingFactor);
    }

    // Draw the lines to generate the arrow
    painter.drawLine(scaledPoints[0], scaledPoints[1]);
    painter.drawLine(scaledPoints[1], scaledPoints[2]);

    // Now add the text
    // Restore the painter object before writing the text
    painter.restore();

    QFont font = this->font();
    font.setPixelSize(s_BUTTON_PIXEL_FONT_SIZE * scalingFactor);
    painter.setFont(font);
    pen.setColor(Qt::GlobalColor::black);
    painter.setPen(pen);
    painter.drawText((m_size + s_TEXT_OFFSET_X) * scalingFactor, (m_size/2 + s_TEXT_OFFSET_Y) * scalingFactor, m_text);

    painter.end();
}

//-----------------------------------------------------------------------------
/// Set the color of the widget.
/// \param color Color of the widget
//-----------------------------------------------------------------------------
void ArrowIconWidget::SetColor(const QColor& color)
{
    m_color = color;

    update();
}

//-----------------------------------------------------------------------------
/// Set the size of the encompassing rectangle.
/// \param size The size of the encompassing rectangle.
//-----------------------------------------------------------------------------
void ArrowIconWidget::SetSize(int size)
{
    m_size = size;

    // Update the vertices
    CreateVertices();

    update();
}

//-----------------------------------------------------------------------------
/// Set the direction of the arrow.
/// \param direction The direction of the arrow.
//-----------------------------------------------------------------------------
void ArrowIconWidget::SetDirection(Direction direction)
{
    m_direction = direction;

    update();
}

//-----------------------------------------------------------------------------
/// Set the pen width of the arrow.
/// \param width The pen width of the arrow.
//-----------------------------------------------------------------------------
void ArrowIconWidget::SetPenWidth(const int width)
{
    m_penWidth = width;

    update();
}

//-----------------------------------------------------------------------------
/// Set the text for this widget.
/// \param text The pen width of the arrow.
//-----------------------------------------------------------------------------
void ArrowIconWidget::setText(const QString& text)
{
    m_text = text;

    update();
}

//-----------------------------------------------------------------------------
/// Create the vertices for the arrow.
//-----------------------------------------------------------------------------
void ArrowIconWidget::CreateVertices()
{
    // Generate the vertices from the size input
    // Vertex zero is at the top left
    m_vertices[0].setX(m_size*.8);
    m_vertices[0].setY(m_size * .6);

    // Vertex 1 is halfway down the bottom side
    m_vertices[1].setX(m_size / 2);
    m_vertices[1].setY(m_size * .9);

    // Vertex 2 is the top right
    m_vertices[2].setX(m_size*.2);
    m_vertices[2].setY(m_size * .6);
}
