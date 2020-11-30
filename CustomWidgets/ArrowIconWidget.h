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

// struct for highlight data
struct StringHighlightData
{
    // The start location for highlighting
    int m_startLocation;

    // The end location for highlighting
    int m_endLocation;

    // The highlighting color
    QColor m_highlightColor;

    // The highlight string
    std::string m_highlightString;
};

/// Class definition to draw an arrow widget
class ArrowIconWidget : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor fontColor READ GetFontColor WRITE SetFontColor)

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
    void SetFontColor(const QColor& color);
    void SetSize(int size);
    void SetDirection(Direction direction);
    void SetPenWidth(int penWidth);
    void SetFontSize(int fontSize);
    void SetHighLightSubString(bool value);
    void SetHighLightSubStringData(QVector<StringHighlightData> stringHighlightData);
    void ClearHighLightSubStringData();
    QColor GetFontColor() const { return m_fontColor; }
    QString GetText();
    void SetBorderColor(const QColor& color);
    void SetShowBorder(bool value);

signals:
    void FocusInEvent();
    void FocusOutEvent();

protected:
    // Qt's paintEvent
    void paintEvent(QPaintEvent* pEvent) Q_DECL_OVERRIDE;

    // Re-implement focus in event
    void focusInEvent(QFocusEvent* pEvent) Q_DECL_OVERRIDE;

    // Re-implement focus out event
    void focusOutEvent(QFocusEvent* pEvent) Q_DECL_OVERRIDE;

    // Re-implement sizeHint
    QSize sizeHint() const Q_DECL_OVERRIDE;

private:
    void CreateVertices();

    int                 m_size;                              ///< The size of the encompassing rect
    QColor              m_color;                             ///< The color of the arrow's lines
    QColor              m_borderColor;                       ///< The color of the widget border, if border requested
    bool                m_showBorder;                        ///< The boolean to indicate if border requested
    QColor              m_fontColor;                         ///< The text font color
    Direction           m_direction;                         ///< The direction of the arrow
    int                 m_penWidth;                          ///< The width of the pen
    QString             m_text;                              ///< The text for the widget
    int                 m_fontSize;                          ///< The size of the text font.
    QPointF             m_vertices[s_NUMBER_OF_VERTICES];    ///< The vertices of the arrow
    bool                m_highlightSubString;                ///< The boolean to indicate if highlight requested
    QVector<StringHighlightData> m_stringHighlightData = {}; ///< The vector to keep track of highlight strings
};

#endif // _ARROW_ICON_WIDGET_H_