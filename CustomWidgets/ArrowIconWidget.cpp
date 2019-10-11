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
#include <../Util/QtUtil.h>

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
    QPushButton(pParent),
    m_showBorder(false),
    m_fontSize(s_BUTTON_PIXEL_FONT_SIZE),
    m_highlightSubString(false)
{
    // Set default values
    m_size = s_BUTTON_BASE_SIZE;
    m_direction = Direction::DownArrow;
    m_color = Qt::GlobalColor::gray;
    m_fontColor = Qt::GlobalColor::black;
    m_borderColor = Qt::GlobalColor::black;
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
    font.setPixelSize(m_fontSize * scalingFactor);
    painter.setFont(font);
    pen.setColor(m_fontColor);
    painter.setPen(pen);

    // Highlight substring if it is requested.
    if (m_highlightSubString)
    {
        // Go through all highlight locations.
        for (const auto& stringHighlightData : m_stringHighlightData)
        {
            QString current = m_text.mid(0, stringHighlightData.m_startLocation + 1);
            int initialTextWidth = QtCommon::QtUtil::GetTextWidth(font, current);

            current = m_text.mid(stringHighlightData.m_startLocation,
                                 stringHighlightData.m_endLocation - stringHighlightData.m_startLocation);
            if (!current.isNull())
            {
                int width = QtCommon::QtUtil::GetTextWidth(font, current);
                QRect rect = this->rect();
                rect.setX(rect.x() + m_size + s_TEXT_OFFSET_X + initialTextWidth);
                rect.setWidth(width);
                painter.fillRect(rect, stringHighlightData.m_highlightColor);
            }
        }
    }

    // Draw the text.
    painter.drawText((m_size + s_TEXT_OFFSET_X) * scalingFactor, (m_size / 2 + s_TEXT_OFFSET_Y) * scalingFactor, m_text);

    // Draw the borders of the widget on focus or mouse hover,
    // if it is requested.
    if (m_showBorder && (hasFocus() || underMouse()))
    {
        pen.setColor(m_borderColor);
        pen.setWidth(1 * scalingFactor);
        painter.setPen(pen);
        painter.drawRect(this->rect());
    }

    painter.end();
}

//-----------------------------------------------------------------------------
/// Set the color of the widget border.
/// \param color Color of the widget border
//-----------------------------------------------------------------------------
void ArrowIconWidget::SetBorderColor(const QColor& color)
{
    m_borderColor = color;
}

//-----------------------------------------------------------------------------
/// Set the boolean to indicate if the border should be drawn.
/// \param value The boolean to indicate if the border should be drawn
//-----------------------------------------------------------------------------
void ArrowIconWidget::SetShowBorder(bool value)
{
    m_showBorder = value;
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
/// Set the font color of the widget.
/// \param color Font color of the widget
//-----------------------------------------------------------------------------
void ArrowIconWidget::SetFontColor(const QColor& color)
{
    m_fontColor = color;

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

    updateGeometry();
    update();
}

//-----------------------------------------------------------------------------
/// Set the size of the font.
/// \param size The size of the text font.
//-----------------------------------------------------------------------------
void ArrowIconWidget::SetFontSize(int fontSize)
{
    m_fontSize = fontSize;

    updateGeometry();
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

    updateGeometry();
    update();
}

//-----------------------------------------------------------------------------
/// Get the text for this widget.
/// \return The text for the button.
//-----------------------------------------------------------------------------
QString ArrowIconWidget::GetText()
{
    return m_text;
}

//-----------------------------------------------------------------------------
/// Set the boolean to indicate highlighting of substring.
/// \param value The boolean to indicate if highlight requested.
//-----------------------------------------------------------------------------
void ArrowIconWidget::SetHighLightSubString(bool value)
{
    m_highlightSubString = value;
}

//-----------------------------------------------------------------------------
/// Set substring data.
/// \param startLocation  The start location to highlight substring.
/// \param endLocation    The end location to highlight substring.
/// \param highlightColor The color to use to highlight substring.
//-----------------------------------------------------------------------------
void ArrowIconWidget::SetHighLightSubStringData(QVector<StringHighlightData> stringHighlightData)
{
    m_stringHighlightData = stringHighlightData;
}

//-----------------------------------------------------------------------------
/// Clear highlight substring data.
//-----------------------------------------------------------------------------
void ArrowIconWidget::ClearHighLightSubStringData()
{
    m_stringHighlightData.clear();
    m_stringHighlightData.squeeze();
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

//-----------------------------------------------------------------------------
/// Called when the ArrowIconWidget gets keyboard focus.
/// \param pEvent A QFocusEvent to inform how focus was obtained.
//-----------------------------------------------------------------------------
void ArrowIconWidget::focusInEvent(QFocusEvent* pEvent)
{
    emit FocusInEvent();

    // Pass the event onto the base class.
    QPushButton::focusInEvent(pEvent);
}

//-----------------------------------------------------------------------------
/// Called when the ArrowIconWidget loses keyboard focus.
/// \param pEvent A QFocusEvent to inform why focus was lost.
//-----------------------------------------------------------------------------
void ArrowIconWidget::focusOutEvent(QFocusEvent* pEvent)
{
    emit FocusOutEvent();

    // Pass the event onto the base class.
    QPushButton::focusOutEvent(pEvent);
}

//-----------------------------------------------------------------------------
/// Provides a recommend size for the widget based on its contents.
/// \return A QSize large enough to fit the arrow and text of this widget.
//-----------------------------------------------------------------------------
QSize ArrowIconWidget::sizeHint() const
{
    QSize size = QPushButton::sizeHint();

    // Get the scaling factor
    const double scalingFactor = ScalingManager::Get().GetScaleFactor();

    // Get font metric
    QFont font = this->font();
    font.setPixelSize(m_fontSize * scalingFactor);
    QFontMetrics fm(font);

    // Height is either: the scaled size of the arrow, or the height of the text, whichever is greater.
    size.setHeight(std::max<int>(m_size*scalingFactor, fm.height()));

    // Width is the width of the scaled arrow + scaled offset + length of text
    // + (another scaled text offset for after the API PSO hash text).
    size.setWidth(m_size*scalingFactor + 2.0 * s_TEXT_OFFSET_X * scalingFactor + fm.width(m_text));

    return size;
}
