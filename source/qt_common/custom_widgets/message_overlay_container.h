//=============================================================================
// Copyright (c) 2021-2025 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief MessageOverlayContainer class definition
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_MESSAGE_OVERLAY_CONTAINER_H_
#define QTCOMMON_CUSTOM_WIDGETS_MESSAGE_OVERLAY_CONTAINER_H_

#include <memory>
#include <functional>
#include <deque>
#include <mutex>
#include <set>

#include <QTimer>
#include <QWidget>
#include <QString>

#include "message_overlay.h"

class MessageOverlayContainer : public QWidget
{
    Q_OBJECT
public:
    /// @brief Constructor
    /// @param parent The parent widget
    /// @param background The background widget
    explicit MessageOverlayContainer(QWidget* parent = nullptr);

    /// @brief Gets the global message overlay container
    /// @return MessageOverlayContainer The widget that is the message overlay container
    static MessageOverlayContainer* Get();

    /// @brief Force close all messages
    void Close();

    /// @brief Sets the overlay background widget to be blurred
    /// @param background The widget to blur
    void SetBackground(QWidget* background);

signals:
    /// @brief Signals message overlay was shown
    void MessageOverlayShown();

private slots:

    /// @brief Called when the queue timer actives and processes the next item in the overlay queue.
    ///
    /// If a message overlay is already being presented, this just returns.
    void ProcessQueue();

    /// @brief Called when the active overlay is closed by the user.
    /// @param result The result of the overlay.
    void OverlayFinished(int result);

protected:
    /// @brief Resize event override
    /// Handles resizing of the active message overlay
    /// @param event The resize event
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

private:
    /// @brief Enable or disable the blur effect
    /// @param enable Boolean for whether to enable or disable
    void SetEnableBlur(bool enable);

    /// @brief Shows the message overlay.
    /// @param title The messageoverlay title.
    /// @param text The messageoverlay text.
    /// @param buttons The messageoverlay buttons.
    /// @param default_button The default button.
    /// @param type The messageoverlay type.
    /// @return user chosen dialog option.
    /// @deprecated This method can result in multiple overlays stacking on top of each other as well as other issues. Prefer to use
    ///             ShowMessageOverlayAsync, especially if you do not need a response from the overlay.
    QDialogButtonBox::StandardButton ShowMessageOverlay(const QString&                    title,
                                                        const QString&                    text,
                                                        QDialogButtonBox::StandardButtons buttons,
                                                        QDialogButtonBox::StandardButton  default_button,
                                                        MessageOverlay::Type              type);

    /// @brief Shows the message overlay, but does not wait for the response from the overlay,
    /// @param title The messageoverlay title.
    /// @param text The messageoverlay text.
    /// @param buttons The messageoverlay buttons.
    /// @param default_button The default button.
    /// @param type The messageoverlay type.
    /// @param key An optional key for the overlay. If there are any over pending overlays with the same key, a new overlay will not be added to the queue.
    ///            This should only be used if you don't need the result from the overlay, otherwise use empty string for the key.
    /// @param callback The function to call with the result of the message overlay after it is shown and the user interacts with it.
    void ShowMessageOverlayAsync(const QString&                                        title,
                                 const QString&                                        text,
                                 QDialogButtonBox::StandardButtons                     buttons,
                                 QDialogButtonBox::StandardButton                      default_button,
                                 MessageOverlay::Type                                  type,
                                 QString                                               key,
                                 std::function<void(QDialogButtonBox::StandardButton)> callback);

    QWidget* background_;  ///< Background widget to display message over
    bool     quitting_;    ///< Quit flag

    /// @brief Class to store a callback with a message overlay in the queue.
    struct MessageOverlayQueueItem
    {
        std::function<void(QDialogButtonBox::StandardButton)> callback;  ///< The callback to be called when the overlay is finished.
        std::shared_ptr<MessageOverlay>                       overlay;   ///< The message overlay to show.
        QString key;  ///< Optional key for the overlay. If an overlay with the key is already in the queue, a new one won't be added.
    };

    std::deque<MessageOverlayQueueItem>       message_overlay_queue_;                ///< The queue of pending message overlays to show.
    std::set<std::shared_ptr<MessageOverlay>> active_overlays_;                      ///< The message overlays that are currently being shown.
    bool                                      has_active_overlay           = false;  ///< true if there is an active overlay, false otherwise.
    int                                       num_sync_presented_overlays_ = 0;  ///< The number of synchronously presented overlays. This can be more than one.

    QTimer queue_timer_;  ///< The timer to control showing the messages inside of the queue.

    friend class MessageOverlay;
};

#endif
