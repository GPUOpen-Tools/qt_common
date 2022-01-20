//=============================================================================
// Copyright (c) 2021 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief MessageOverlayContainer class implementation
//=============================================================================

#include "message_overlay_container.h"

#include <stdexcept>

#include <QApplication>
#include <QGraphicsEffect>
#include <QThread>

MessageOverlayContainer::MessageOverlayContainer(QWidget* parent)
    : QWidget(parent)
    , quitting_(false)
    , active_overlay_(nullptr)
{
    // Check if existing instance is already alive
    // and disallow creating more than 1 instance of this class
    // NOTE:
    // We could have used a singleton, but its preferable to have
    // this class lifetime managed by Qt
    MessageOverlayContainer* container = Get();
    Q_ASSERT(container == this);
}

MessageOverlayContainer* MessageOverlayContainer::Get()
{
    MessageOverlayContainer* container = nullptr;

    auto widgets = qApp->allWidgets();
    for (auto& w : widgets)
    {
        container = qobject_cast<MessageOverlayContainer*>(w);
        if (container != nullptr)
        {
            break;
        }
    }

    return container;
}

void MessageOverlayContainer::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    if(active_overlay_ != nullptr)
    {
        active_overlay_->setGeometry(this->geometry());
    }
}

void MessageOverlayContainer::Close()
{
    quitting_ = true;
}

void MessageOverlayContainer::SetBackground(QWidget* background)
{
    background_ = background;
}

void MessageOverlayContainer::SetEnableBlur(bool enable)
{
    Q_ASSERT(background_ != nullptr);
    if (background_ != nullptr)
    {
        if (enable)
        {
            auto* blur = new QGraphicsBlurEffect();
            blur->setBlurRadius(15.0f);
            background_->setGraphicsEffect(blur);
        }
        else
        {
            // The blur effect doesn't need to be deleted manually.
            // Qt will delete it under the hood before using the null effect.
            background_->setGraphicsEffect(nullptr);
        }
    }
}

QDialogButtonBox::StandardButton MessageOverlayContainer::ShowMessageOverlay(const QString&                    title,
                                                                             const QString&                    text,
                                                                             QDialogButtonBox::StandardButtons buttons,
                                                                             QDialogButtonBox::StandardButton  default_button,
                                                                             MessageOverlay::Type              type)
{
    // Show the message overlay
    SetEnableBlur(true);

    MessageOverlay message_overlay(Get());
    message_overlay.SetTitle(title);
    message_overlay.SetText(text);
    message_overlay.SetType(type);
    message_overlay.SetButtons(buttons);
    message_overlay.SetDefaultButton(default_button);
    message_overlay.resize(Get()->geometry().size());
    message_overlay.move(Get()->geometry().topLeft());

    emit MessageOverlayShown();

    active_overlay_ = &message_overlay;
    message_overlay.exec();
    active_overlay_ = nullptr;

    if (quitting_)
    {
        return QDialogButtonBox::NoButton;
    }

    // Hide the message overlay
    SetEnableBlur(false);

    return message_overlay.GetResult();
}
