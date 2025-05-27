//=============================================================================
// Copyright (c) 2025 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Declaration for a widget that behaves like a tooltip.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_TOOLTIP_WIDGET_H_
#define QTCOMMON_CUSTOM_WIDGETS_TOOLTIP_WIDGET_H_

#include <vector>

#include <QEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QScrollArea>
#ifdef Q_OS_LINUX
#include <QTimer>
#endif
#include <QWidget>

/// @brief TooltipWidget is a widget that intends to function as a tooltip.
///
/// TooltipWidget should be shown when the mouse is over something special, and hidden otherwise.
/// It can track with the mouse or stand still. Applications should be responsible for showing it or moving it.
/// If given a container widget and scroll areas, it will try to hide itself if the mouse leaves those containers.
class TooltipWidget : public QWidget
{
    Q_OBJECT

public:
    static const int kTooltipDelayMs = 600;  ///< Delay showing the tooltip.
    static const int kTooltipBorder  = 1;    ///< Border around the tooltip to distinguish it from what's behind it.
    static const int kTooltipMargin  = 2;    ///< Margin for the tooltip to give some space between its contents and the border.

    /// @brief Constructor; set flags to turn this widget into a tooltip.
    ///
    /// @param [in] parent                 The parent widget.
    /// @param [in] follow_mouse_position  true to keep this tooltip near the mouse, false to stand still.
    /// @param [in] container_widget       The widget that this tooltip should be confined to.
    /// @param [in] container_scroll_areas The scroll areas that this tooltip should be confined to.
    TooltipWidget(QWidget*                  parent,
                  bool                      follow_mouse_position,
                  QWidget*                  container_widget,
                  std::vector<QScrollArea*> container_scroll_areas = std::vector<QScrollArea*>{});

    /// @brief Destructor.
    ~TooltipWidget();

    /// @brief Move this tooltip to a new position.
    ///
    /// @param [in] new_global_position The global position to move to.
    void UpdatePosition(QPoint new_global_position);

    /// @brief Check if the provided global position is within the geometry of this tooltip's container.
    ///
    /// @param [in] global_position The position to check for; should be in global coordinates.
    ///
    /// @return true if the position is inside this tooltip's container's geometry, false otherwise.
    bool ContainerGeometryContainsPosition(QPoint global_position) const;

    /// @brief Remember any scroll areas that should affect this tooltip's visibility.
    ///
    /// If this tooltip leaves the geometry of a given scroll area this tooltip should be hidden.
    ///
    /// @param [in] container_scroll_areas The scroll areas to remember.
    void RegisterScrollAreas(std::vector<QScrollArea*> container_scroll_areas);

protected:
    /// @brief Override leave to make sure this tooltip is not visible if the mouse leaves its parent's geometry. Starts the hide timer on linux.
    ///
    /// @param [in] event The leave event.
    virtual void leaveEvent(QEvent* event) Q_DECL_OVERRIDE;

#ifdef Q_OS_LINUX
    /// @brief Override enter to stop the hide timer when the mouse enters the tooltip.
    ///
    /// @param [in] event The enter event.
    void enterEvent(QEnterEvent* event) Q_DECL_OVERRIDE;
#endif

    /// @brief Override mouse move to try and prevent this tooltip from getting stuck.
    ///
    /// @param [in] event The mouse event.
    virtual void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

    bool     follow_mouse_position_;  ///< true to keep this tooltip near the mouse, false to stand still.
    QWidget* background_widget_;      ///< An extra intermediate widget to ensure background is not transparent on certain platforms.

#ifdef Q_OS_LINUX
protected slots:
    /// @brief Callback to hide the tooltip once the hider timer timeouts. Needed for Wayland since we cannot use mouse position if the mouse leaves the window.
    void HideTimerCallback();
#endif

private:
    /// @brief Install an event filter to assist hiding this tooltip if the mouse leaves its container widget.
    ///
    /// @param [in] object The object that the event originated from.
    /// @param [in] event  The event.
    ///
    /// @return false. Always return false so that all events, even leave events continue to be processed.
    bool eventFilter(QObject* object, QEvent* event);

    QWidget*                        container_widget_;        ///< The widget that this tooltip should be confined to.
    std::vector<const QScrollArea*> container_scroll_areas_;  ///< The scroll areas that this tooltip should be confined to.
#ifdef Q_OS_LINUX
    QTimer hide_timer_; ///< Timer to hide the tooltip after we get leave events.
#endif
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_TOOLTIP_WIDGET_H_
