//=============================================================================
/// Copyright (c) 2017-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a QPushButton with vector based icons for various states.
//=============================================================================

#include "icon_button.h"

#include <QPaintEvent>
#include <QMouseEvent>
#include <QPushButton>
#include <QIcon>
#include <QStylePainter>
#include <QStylePainter>
#include <QStyleOptionButton>

#include "utils/scaling_manager.h"

IconButton::IconButton(QWidget* parent)
    : QPushButton(parent)
{
    setFlat(true);

    setBaseSize(QSize(kDefaultIconSize, kDefaultIconSize));

    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &IconButton::updateGeometry);
}

IconButton::IconButton(QWidget*       parent,
                       const QString& normal_icon_resource,
                       const QString& hover_icon_resource,
                       const QString& pressed_icon_resource,
                       const QString& disabled_icon_resource)
    : QPushButton(parent)
    , normal_icon_(QIcon(normal_icon_resource))
    , hover_icon_(QIcon(hover_icon_resource))
    , pressed_icon_(QIcon(pressed_icon_resource))
    , disabled_icon_(QIcon(disabled_icon_resource))
{
    setFlat(true);

    setBaseSize(QSize(kDefaultIconSize, kDefaultIconSize));

    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &IconButton::updateGeometry);
}

IconButton::~IconButton()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &IconButton::updateGeometry);
}

void IconButton::SetNormalIcon(const QIcon& icon)
{
    normal_icon_ = icon;
}

void IconButton::SetHoverIcon(const QIcon& icon)
{
    hover_icon_ = icon;
}

void IconButton::SetPressedIcon(const QIcon& icon)
{
    pressed_icon_ = icon;
}

void IconButton::SetDisabledIcon(const QIcon& icon)
{
    disabled_icon_ = icon;
}

QSize IconButton::sizeHint() const
{
    QSize size_hint(baseSize());

    size_hint = ScalingManager::Get().Scaled(size_hint);

    return size_hint;
}

void IconButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QStylePainter painter(this);
    painter.setPen(QPen(palette().color(QPalette::Background)));
    painter.drawLine(rect().topLeft(), rect().topRight());
    if (!isEnabled())
    {
        disabled_icon_.paint(&painter, rect());
    }
    else if (isDown())
    {
        pressed_icon_.paint(&painter, rect());
    }
    else if (testAttribute(Qt::WA_UnderMouse))
    {
        hover_icon_.paint(&painter, rect());
    }
    else
    {
        normal_icon_.paint(&painter, rect());
    }
}

void IconButton::enterEvent(QEvent* event)
{
    QPushButton::enterEvent(event);
    setAttribute(Qt::WA_UnderMouse, true);
}

void IconButton::leaveEvent(QEvent* event)
{
    QPushButton::leaveEvent(event);
    setAttribute(Qt::WA_UnderMouse, false);
}
