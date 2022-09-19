//=============================================================================
// Copyright (c) 2021 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief MessageOverlay class implementation
//=============================================================================

#include "message_overlay.h"

#include <QPushButton>
#include <QThread>

#include "message_overlay_container.h"
#include "ui_message_overlay.h"

MessageOverlay::MessageOverlay(QWidget* parent)
    : QDialog(parent)
    , ui_(new Ui::MessageOverlay)
    , result_(QDialogButtonBox::NoButton)
{
    ui_->setupUi(this);
    ui_->dontAskAgainButton->hide();

    connect(ui_->button_box, &QDialogButtonBox::clicked, this, &MessageOverlay::OnButtonClicked);
    connect(ui_->button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui_->button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);

    setWindowFlags(Qt::WindowType::Widget);
}

MessageOverlay::~MessageOverlay() = default;

void MessageOverlay::OnButtonClicked(QAbstractButton* button)
{
    result_ = ui_->button_box->standardButton(button);
}

void MessageOverlay::SetTitle(const QString& title)
{
    ui_->titleLabel->setText(title);
}

void MessageOverlay::SetText(const QString& text)
{
    ui_->notificationTextLabel->setText(text);
}

void MessageOverlay::SetButtons(QDialogButtonBox::StandardButtons buttons)
{
    ui_->button_box->setStandardButtons(buttons);
}

void MessageOverlay::SetDefaultButton(QDialogButtonBox::StandardButton button)
{
    if (button != QDialogButtonBox::NoButton)
    {
        QPushButton* push_button = ui_->button_box->button(button);
        if (push_button != nullptr)
        {
            push_button->setDefault(true);
        }
    }
}

void MessageOverlay::SetType(Type type)
{
    if (type == Type::Error)
    {
        // red
        ui_->translucentColorSlice->setStyleSheet(QStringLiteral("background-color: rgba(254, 30, 55, 128);"));
    }
    else if (type == Type::Info)
    {
        // blue
        ui_->translucentColorSlice->setStyleSheet(QStringLiteral("background-color: rgba(88, 166, 255, 128);"));
    }
    else
    {
        // yellow
        ui_->translucentColorSlice->setStyleSheet(QStringLiteral("background-color: rgba(255, 240, 0, 128);"));
    }
}

QDialogButtonBox::StandardButton MessageOverlay::GetResult() const
{
    return result_;
}

QDialogButtonBox::StandardButton MessageOverlay::Critical(const QString&                    title,
                                                          const QString&                    text,
                                                          QDialogButtonBox::StandardButtons buttons,
                                                          QDialogButtonBox::StandardButton  default_button)
{
    MessageOverlayContainer* container = MessageOverlayContainer::Get();
    Q_ASSERT(container != nullptr);
    if (container != nullptr)
    {
        return container->ShowMessageOverlay(title, text, buttons, default_button, MessageOverlay::Type::Error);
    }

    return QDialogButtonBox::StandardButton::NoButton;
}

QDialogButtonBox::StandardButton MessageOverlay::Warning(const QString&                    title,
                                                         const QString&                    text,
                                                         QDialogButtonBox::StandardButtons buttons,
                                                         QDialogButtonBox::StandardButton  default_button)
{
    MessageOverlayContainer* container = MessageOverlayContainer::Get();
    Q_ASSERT(container != nullptr);
    if (container != nullptr)
    {
        return container->ShowMessageOverlay(title, text, buttons, default_button, MessageOverlay::Type::Warning);
    }

    return QDialogButtonBox::StandardButton::NoButton;
}

QDialogButtonBox::StandardButton MessageOverlay::Info(const QString&                    title,
                                                      const QString&                    text,
                                                      QDialogButtonBox::StandardButtons buttons,
                                                      QDialogButtonBox::StandardButton  default_button)
{
    MessageOverlayContainer* container = MessageOverlayContainer::Get();
    Q_ASSERT(container != nullptr);
    if (container != nullptr)
    {
        return container->ShowMessageOverlay(title, text, buttons, default_button, MessageOverlay::Type::Info);
    }

    return QDialogButtonBox::StandardButton::NoButton;
}

QDialogButtonBox::StandardButton MessageOverlay::Question(const QString&                    title,
                                                          const QString&                    text,
                                                          QDialogButtonBox::StandardButtons buttons,
                                                          QDialogButtonBox::StandardButton  default_button)
{
    MessageOverlayContainer* container = MessageOverlayContainer::Get();
    Q_ASSERT(container != nullptr);
    if (container != nullptr)
    {
        return container->ShowMessageOverlay(title, text, buttons, default_button, MessageOverlay::Type::Question);
    }

    return QDialogButtonBox::StandardButton::NoButton;
}
