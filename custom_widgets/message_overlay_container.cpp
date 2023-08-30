//=============================================================================
// Copyright (c) 2021-2022 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief MessageOverlayContainer class implementation
//=============================================================================

#include "message_overlay_container.h"

#include <stdexcept>

#include <QApplication>
#include <QGraphicsEffect>
#include <QThread>

/// @brief The number of milliseconds between each poll of the message overlay queue.
static constexpr int kMessageOverlayQueuePollInterval = 50;

MessageOverlayContainer::MessageOverlayContainer(QWidget* parent)
    : QWidget(parent)
    , quitting_(false)
{
    // Check if existing instance is already alive
    // and disallow creating more than 1 instance of this class
    // NOTE:
    // We could have used a singleton, but its preferable to have
    // this class lifetime managed by Qt
    MessageOverlayContainer* container = Get();
    Q_ASSERT(container == this);

    connect(&queue_timer_, &QTimer::timeout, this, &MessageOverlayContainer::ProcessQueue);
    queue_timer_.setInterval(kMessageOverlayQueuePollInterval);
    queue_timer_.start();
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

void MessageOverlayContainer::ProcessQueue()
{
    if (has_active_overlay || message_overlay_queue_.empty() || num_sync_presented_overlays_ != 0)
    {
        return;
    }

    std::shared_ptr<MessageOverlay>& overlay = message_overlay_queue_.front().overlay;
    has_active_overlay                       = true;

    overlay->setVisible(true);
    overlay->resize(Get()->geometry().size());
    overlay->move(Get()->geometry().topLeft());

    connect(overlay.get(), &QDialog::finished, this, &MessageOverlayContainer::OverlayFinished);

    emit MessageOverlayShown();
    SetEnableBlur(true);
    overlay->open();
}

void MessageOverlayContainer::OverlayFinished(int result)
{
    Q_UNUSED(result);

    MessageOverlayQueueItem& item = message_overlay_queue_.front();
    if (num_sync_presented_overlays_ == 0)
    {
        SetEnableBlur(false);
    }

    active_overlays_.erase(item.overlay);

    if (item.callback)
    {
        item.callback(item.overlay->GetResult());
    }

    has_active_overlay = false;
    message_overlay_queue_.pop_front();
}

void MessageOverlayContainer::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    for (auto i = active_overlays_.begin(); i != active_overlays_.end(); i++)
    {
        (*i)->setGeometry(geometry());
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
    auto message_overlay = std::make_shared<MessageOverlay>(Get());
    active_overlays_.insert(message_overlay);

    message_overlay->SetTitle(title);
    message_overlay->SetText(text);
    message_overlay->SetType(type);
    message_overlay->SetButtons(buttons);
    message_overlay->SetDefaultButton(default_button);

    message_overlay->resize(Get()->geometry().size());
    message_overlay->move(Get()->geometry().topLeft());

    SetEnableBlur(true);
    ++num_sync_presented_overlays_;

    emit MessageOverlayShown();
    message_overlay->exec();

    --num_sync_presented_overlays_;
    active_overlays_.erase(message_overlay);

    if (num_sync_presented_overlays_ == 0 && !has_active_overlay)
    {
        SetEnableBlur(false);
    }

    if (quitting_)
    {
        return QDialogButtonBox::NoButton;
    }

    return message_overlay->GetResult();
}

void MessageOverlayContainer::ShowMessageOverlayAsync(const QString&                                        title,
                                                      const QString&                                        text,
                                                      QDialogButtonBox::StandardButtons                     buttons,
                                                      QDialogButtonBox::StandardButton                      default_button,
                                                      MessageOverlay::Type                                  type,
                                                      QString                                               key,
                                                      std::function<void(QDialogButtonBox::StandardButton)> callback)
{
    if (!key.isEmpty())
    {
        for (auto i = message_overlay_queue_.begin(); i != message_overlay_queue_.end(); i++)
        {
            if ((*i).key == key)
            {
                return;
            }
        }
    }

    auto message_overlay = std::make_shared<MessageOverlay>(Get());
    active_overlays_.insert(message_overlay);

    message_overlay->SetTitle(title);
    message_overlay->SetText(text);
    message_overlay->SetType(type);
    message_overlay->SetButtons(buttons);
    message_overlay->SetDefaultButton(default_button);
    message_overlay->setVisible(false);

    MessageOverlayQueueItem item;
    item.overlay  = std::move(message_overlay);
    item.callback = std::move(callback);
    item.key      = key;

    message_overlay_queue_.push_back(item);
}