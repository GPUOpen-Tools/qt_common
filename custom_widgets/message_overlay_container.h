//=============================================================================
// Copyright (c) 2021 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief MessageOverlayContainer class definition
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_MESSAGE_OVERLAY_CONTAINER_H_
#define QTCOMMON_CUSTOM_WIDGETS_MESSAGE_OVERLAY_CONTAINER_H_

#include <memory>

#include <QWidget>

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

protected:
    /// @brief Resize event override
    /// Handles resizing of the active message overlay
    /// @param event The resize event
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

private:
    /// @brief Enable or disable the blur effect
    /// @param enable Boolean for whether to enable or disable
    void SetEnableBlur(bool enable);

    /// @brief Shows the message overlay
    /// @param title The messageoverlay title
    /// @param text The messageoverlay text
    /// @param buttons The messageoverlay buttons
    /// @param default_button The default button
    /// @param type The messageoverlay type
    /// @return user chosen dialog option
    QDialogButtonBox::StandardButton ShowMessageOverlay(const QString&                    title,
                                                        const QString&                    text,
                                                        QDialogButtonBox::StandardButtons buttons,
                                                        QDialogButtonBox::StandardButton  default_button,
                                                        MessageOverlay::Type              type);

    QWidget*        background_;      ///< Background widget to display message over
    bool            quitting_;        ///< Quit flag
    MessageOverlay* active_overlay_;  ///< Handle to active message overlay

    friend class MessageOverlay;
};

#endif
