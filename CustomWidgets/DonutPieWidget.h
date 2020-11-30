//=============================================================================
/// Copyright (c) 2016-2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for a donut pie graphics item widget.
//=============================================================================

#ifndef _DONUT_PIE_WIDGET_H_
#define _DONUT_PIE_WIDGET_H_

#include <QGraphicsItem>

/// Support for a donut pie graphics item widget.
class DonutPieWidget : public QGraphicsItem
{
public:
    // Implementations for Qt
    explicit DonutPieWidget(int width, int height);
    virtual ~DonutPieWidget() {}
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget);

    // Custom implementations
    void setNumSegments(unsigned int numSegments);
    void setIndexValue(unsigned int index, qreal value);
    void setIndexColor(unsigned int index, const QColor& fillColor);
    void setIndexText(unsigned int index, const QString& text);
    void setArcWidth(qreal arcWidth);
    void setTextLineOne(const QString& text);
    void setTextLineTwo(const QString& text);
    void setBackgroundColor(const QColor& color);

private:
    class SliceData
    {
    public:
        SliceData() : m_value(0), m_fillColor(Qt::black), m_sliceText()
        {}
        ~SliceData() {}

        qreal   m_value;                    ///< current control value to represent
        QColor  m_fillColor;                ///< color used to fill the value part of the arc.
        QString m_sliceText;                     ///< Additional text description
    };

    QVector<SliceData> m_slices;

    int              m_width;               ///< width of this control
    int              m_height;              ///< height of this control
    unsigned int     m_numSegments;         ///< number of segments (values) in the pie control
    qreal            m_arcWidth;            ///< width of the control arc, in pixels. This is used as the size of the pen used to draw the arc
    QString          m_textLineOne;         ///< text in the center of the donut
    QString          m_textLineTwo;         ///< text in the center of the donut
    QColor           m_backgroundColor;     ///< set the background color
};

#endif // _DONUT_PIE_WIDGET_H_
