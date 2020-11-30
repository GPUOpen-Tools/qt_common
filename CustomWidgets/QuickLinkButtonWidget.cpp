//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a quick links QPushButton
//=============================================================================

#include "QuickLinkButtonWidget.h"

#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionButton>

#include <../Scaling/ScalingManager.h>

static const int s_BASE_WIDTH = 325;
static const int s_BASE_HEIGHT = 60;
static const int s_BASE_FONT_SIZE = 10;
static const int s_BASE_MARGIN_LENGTH = 5;
static const QColor s_BACKGROUND_COLOR = QColor(241, 241, 241);
static const QColor s_HIGHLIGHT_COLOR = QColor(0, 0, 255);

//-----------------------------------------------------------------------------
/// Explicit constructor
/// \param pParent The parent widget
//-----------------------------------------------------------------------------
QuickLinkButtonWidget::QuickLinkButtonWidget(QWidget* pParent)
    : QPushButton("test", pParent),
    m_titleString(""),
    m_descLineOne(""),
    m_descLineTwo(""),
    m_textColor(Qt::black)
{
    setCursor(Qt::PointingHandCursor);

    m_scaleFactor = ScalingManager::Get().GetScaleFactor();

    setMinimumWidth(s_BASE_WIDTH);
    setMinimumHeight(s_BASE_HEIGHT);
    setMaximumWidth(s_BASE_WIDTH);
    setMaximumHeight(s_BASE_HEIGHT);
}

//-----------------------------------------------------------------------------
/// Overridden paint event for this QPushButton.
/// \param pEvent The paint event
//-----------------------------------------------------------------------------
void QuickLinkButtonWidget::paintEvent(QPaintEvent* pEvent)
{
    Q_UNUSED(pEvent);

    QPainter painter(this);

    painter.setPen(s_BACKGROUND_COLOR);
    painter.setBrush(s_BACKGROUND_COLOR);
    painter.drawRect(0, 0, s_BASE_WIDTH * m_scaleFactor, s_BASE_HEIGHT * m_scaleFactor);

    const int yOffset = ((s_BASE_HEIGHT - s_BASE_MARGIN_LENGTH*2) / 3) * m_scaleFactor;

    QFont font;
    painter.setPen(m_textColor);
    font.setPixelSize(s_BASE_FONT_SIZE * m_scaleFactor);
    font.setBold(true);

    painter.setFont(font);
    painter.drawText(s_BASE_MARGIN_LENGTH * m_scaleFactor, yOffset, m_titleString);

    font.setBold(false);
    painter.setFont(font);
    painter.drawText(s_BASE_MARGIN_LENGTH * m_scaleFactor, yOffset*2, m_descLineOne);
    painter.drawText(s_BASE_MARGIN_LENGTH * m_scaleFactor, yOffset*3, m_descLineTwo);
}

//-----------------------------------------------------------------------------
/// Overridden mouse hover leave event
/// \param pEvent The event
//-----------------------------------------------------------------------------
void QuickLinkButtonWidget::leaveEvent(QEvent* pEvent)
{
    QPushButton::leaveEvent(pEvent);

    m_textColor = Qt::black;
}

//-----------------------------------------------------------------------------
/// Overridden mouse hover enter event
/// \param pEvent The event
//-----------------------------------------------------------------------------
void QuickLinkButtonWidget::enterEvent(QEvent* pEvent)
{
    QPushButton::enterEvent(pEvent);

    m_textColor = s_HIGHLIGHT_COLOR;
}