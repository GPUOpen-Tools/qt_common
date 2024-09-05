//=============================================================================
// Copyright (c) 2024 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief  Header for the custom banner widget.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_BANNER_WIDGET_H_
#define QTCOMMON_CUSTOM_WIDGETS_BANNER_WIDGET_H_

#include <QString>
#include <QWidget>

namespace Ui
{
    class BannerWidget;
}

/// @brief A custom widget for displaying a banner with a message and links.
class BannerWidget : public QWidget
{
    Q_OBJECT

public:
    /// @brief Constructor
    ///
    /// @param [in] parent                          The parent of the banner widget.
    explicit BannerWidget(QWidget* parent = nullptr);

    /// @brief Destructor.
    virtual ~BannerWidget();

    /// @brief Set the text for the notification.
    ///
    /// @param [in] notification_text               The text to display in the notification.
    void SetNotificationText(const QString& notification_text);

    /// @brief Set text color used for links on the banner.
    ///
    /// @param [in] color                           The text color for the link.
    void SetLinkColor(const QColor& color);

    /// @brief Set the text color used for disabled links on the banner.
    ///
    /// @param [in] color                           The text color for the disabled link.
    void SetDisabledLinkColor(const QColor& color);

    /// @brief Emits a signal indicating that the user has clicked the "Do not show again" link.
    virtual void DontShowAgainQuery();

protected:
    /// @brief Overridden paint event for this widget.
    ///
    /// @param event The paint event
    virtual void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

    /// @brief Event filter for the banner widget.
    ///
    /// Handles updating the text color for child labels.
    ///
    /// @param [in] object                         The object that the event is for.
    /// @param [in] event                          The event that occurred.
    ///
    /// @return True if the event was handled, false otherwise.
    bool eventFilter(QObject* object, QEvent* event);

signals:
    /// @brief Signal emitted when the "show details" link is clicked.
    void ShowDetailsClicked();

    /// @brief Signal emitted when the "Do not show again" link is clicked.
    void DontShowAgainRequested();

    /// @brief Signal emitted when the close button is clicked.
    void CloseClicked();

private slots:
    /// @brief Slot to handle the show details link being clicked.
    ///
    /// @param link                                 The link that was clicked.
    void HandleShowDetailsClicked(const QString& link);

    /// @brief Slot to handle the close button being clicked.
    ///
    /// @param link                                 The link that was clicked.
    void HandleCloseClicked(const QString& link);

    /// @brief Slot to handle the "don't show again" link being clicked.
    ///
    /// @param link                                 The link that was clicked.
    void HandleDontShowAgainClicked(const QString& link);

private:
    /// @brief Intializes the widget, adds subwidgets and inserts the banner into a parent layout if one exists.
    void Init();

protected:
    Ui::BannerWidget* ui_;  ///< The Qt ui form.
};
#endif  // QTCOMMON_CUSTOM_WIDGETS_BANNER_WIDGET_H_
