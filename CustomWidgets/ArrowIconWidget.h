//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for the up/down arrow widget.
//=============================================================================

#ifndef _ARROW_ICON_WIDGET_H_
#define _ARROW_ICON_WIDGET_H_

#include <QPushButton>

static const int s_NUMBER_OF_VERTICES = 3;

/// Class definition to draw an arrow widget
class ArrowIconWidget : public QPushButton
{
public:
    // enum to specify up/down arrow
    enum Direction
    {
        DownArrow,
        UpArrow
    };

    // Implementations for Qt
    explicit ArrowIconWidget(QWidget* pParent = nullptr);
    virtual ~ArrowIconWidget() {}
    void setText(const QString& string);

    // Custom implementations
    void SetColor(const QColor& color);
    void SetSize(int size);
    void SetDirection(Direction direction);
    void SetPenWidth(int penWidth);

protected:
    // Qt's paintEvent
    virtual void paintEvent(QPaintEvent* pEvent);

private:
    void CreateVertices();

    int                 m_size;                             ///< The size of the encompassing rect
    QColor              m_color;                            ///< The color of the arrow's lines
    Direction           m_direction;                        ///< The direction of the arrow
    int                 m_penWidth;                         ///< The width of the pen
    QString             m_text;                             ///< The text for the widget
    QPointF             m_vertices[s_NUMBER_OF_VERTICES];   ///< The vertices of the arrow
};

#endif // _ARROW_ICON_WIDGET_H_