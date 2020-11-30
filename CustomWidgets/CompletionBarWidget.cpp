//=============================================================================
/// Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a completion bar.
//=============================================================================

#include "CompletionBarWidget.h"

#include <QPainter>

static const QColor s_EMPTY_COLOR(204,204,204);
static const QColor s_FILL_COLOR(0,118,215);
static const int s_DEFAULT_WIDTH = 325;
static const int s_DEFAULT_HIGHT = 20;

//-----------------------------------------------------------------------------
/// Explicit constructor
/// \param fillPct How much of this widget to fill in?
//-----------------------------------------------------------------------------
CompletionBarWidget::CompletionBarWidget(qreal fillPct) :
    m_width(s_DEFAULT_WIDTH),
    m_height(s_DEFAULT_HIGHT),
    m_fillPct(fillPct)
{
}

//-----------------------------------------------------------------------------
/// Explicit constructor
/// \param width The widget's width
/// \param height The widget's height
/// \param fillPct How much of this widget to fill in?
//-----------------------------------------------------------------------------
CompletionBarWidget::CompletionBarWidget(int width, int height, qreal fillPct) :
    m_width(width),
    m_height(height),
    m_fillPct(fillPct)
{
}

//-----------------------------------------------------------------------------
/// Implementation of Qt's bounding volume for this item.
/// \return The item's bounding rectangle.
//-----------------------------------------------------------------------------
QRectF CompletionBarWidget::boundingRect() const
{
    return QRectF(0, 0, m_width, m_height);
}

//-----------------------------------------------------------------------------
/// Implementation of Qt's paint for this item.
/// \param pPainter The painter object to use.
/// \param pOption Provides style options for the item, such as its state, exposed area and its level-of-detail hints.
/// \param pWidget Points to the widget that is being painted on if specified.
//-----------------------------------------------------------------------------
void CompletionBarWidget::paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget)
{
    Q_UNUSED(pOption);
    Q_UNUSED(pWidget);

    QRectF rect = boundingRect();

    QBrush brush(s_EMPTY_COLOR);

    QPen pen(Qt::NoPen);
    pPainter->setPen(pen);
    pPainter->fillRect(rect, brush);
    pPainter->drawRect(rect);

    rect.setRight(rect.right() * (m_fillPct/100.0f));
    brush.setColor(s_FILL_COLOR);

    pPainter->setBrush(brush);
    pPainter->fillRect(rect, brush);
    pPainter->drawRect(rect);
}

//-----------------------------------------------------------------------------
/// Set how far to fill the bar. Should be 1-100.
/// \param fillPct How much of this widget to fill in?
//-----------------------------------------------------------------------------
void CompletionBarWidget::setFillPct(qreal fillPct)
{
    m_fillPct = fillPct;
}
