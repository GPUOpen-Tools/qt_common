//=============================================================================
/// Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for a completion bar.
//=============================================================================

#ifndef _COMPLETION_BAR_WIDGET_H_
#define _COMPLETION_BAR_WIDGET_H_

#include <QGraphicsItem>

/// Support for the RGP-style completion bar.
class CompletionBarWidget : public QGraphicsItem
{
public:
    // Implementations for Qt
    explicit CompletionBarWidget(qreal fillPct);
    explicit CompletionBarWidget(int width, int height, qreal fillPct);
    virtual ~CompletionBarWidget() {}
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget);

    // Custom implementations
    void setFillPct(qreal fillPct);

private:
    int     m_width;            ///< Width of this control
    int     m_height;           ///< Height of this control
    qreal   m_fillPct;          ///< Percentage of the bar that was filled in
};

#endif // _COMPLETION_BAR_WIDGET_H_
