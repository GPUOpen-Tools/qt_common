//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a QPushButton with vector based icons for various states.
//=============================================================================

#include <QPaintEvent>
#include <QMouseEvent>
#include <QPushButton>
#include <QIcon>
#include <QStylePainter>
#include <QStylePainter>
#include <QStyleOptionButton>

#include "IconButton.h"

//-----------------------------------------------------------------------------
/// constructor for the icon button.
/// \param pParent Pointer to the parent widget.
/// \param normalIconResource The default icon resource string for the button.
/// \param hoverIconResource The resource string of the icon used when the mouse
/// is over the button.
/// \param pressedIconResource resource string of the icon used when the button is pressed.
/// \param disabledIconResource resource string of the icon used when the button is disabled.
//-----------------------------------------------------------------------------
IconButton::IconButton(QWidget* pParent, const QString& normalIconResource, const QString& hoverIconResource, const QString& pressedIconResource, const QString& disabledIconResource)
    : QPushButton(pParent),
    m_normalIcon(QIcon(normalIconResource)),
    m_hoverIcon(QIcon(hoverIconResource)),
    m_pressedIcon(QIcon(pressedIconResource)),
    m_disabledIcon(QIcon(disabledIconResource))
{
    setFlat(true);
}

//-----------------------------------------------------------------------------
/// Set the default icon for the button.
/// \param reference to the icon.
//-----------------------------------------------------------------------------
void IconButton::SetNormalIcon(const QIcon& icon)
{
    m_normalIcon = icon;
}

//-----------------------------------------------------------------------------
/// Set the icon used when the mouse hovers over the button.
/// \param reference to the icon.
//-----------------------------------------------------------------------------
void IconButton::SetHoverIcon(const QIcon& icon)
{
    m_hoverIcon = icon;
}

//-----------------------------------------------------------------------------
/// Set the icon used when the button is pressed.
/// \param reference to the icon.
//-----------------------------------------------------------------------------
void IconButton::SetPressedIcon(const QIcon& icon)
{
    m_pressedIcon = icon;
}

//-----------------------------------------------------------------------------
/// Set the icon used when the button is disabled.
/// \param reference to the icon.
//-----------------------------------------------------------------------------
void IconButton::SetDisabledIcon(const QIcon& icon)
{
    m_disabledIcon = icon;
}

//-----------------------------------------------------------------------------
/// Overriden paint event for this IconButton.
/// \param pEvent Pointer to the QPaintEvent object.
//-----------------------------------------------------------------------------
void IconButton::paintEvent(QPaintEvent* pEvent)
{
    QStylePainter painter(this);
    painter.setPen(QPen(palette().color(QPalette::Background)));
    painter.drawLine(pEvent->rect().topLeft(), pEvent->rect().topRight());
    if (!isEnabled())
    {
        m_disabledIcon.paint(&painter, pEvent->rect());
    }
    else if (isDown())
    {
        m_pressedIcon.paint(&painter, pEvent->rect());
    }
    else if (testAttribute(Qt::WA_UnderMouse))
    {
        m_hoverIcon.paint(&painter, pEvent->rect());
    }
    else
    {
        m_normalIcon.paint(&painter, pEvent->rect());
    }
}

//-----------------------------------------------------------------------------
/// Event triggered when the mouse is over the button.
/// \param pEvent Pointer to the event object.
//-----------------------------------------------------------------------------
void IconButton::enterEvent(QEvent* pEvent)
{
    QPushButton::enterEvent(pEvent);
    setAttribute(Qt::WA_UnderMouse, true);
}

//-----------------------------------------------------------------------------
/// Event triggered when the mouse is no longer over the button.
/// \param pEvent Pointer to the event object.
//-----------------------------------------------------------------------------
void IconButton::leaveEvent(QEvent* pEvent)
{
    QPushButton::leaveEvent(pEvent);
    setAttribute(Qt::WA_UnderMouse, false);
}

