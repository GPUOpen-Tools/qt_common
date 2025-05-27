//=============================================================================
// Copyright (c) 2025 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Implementation for a widget that behaves like a tooltip.
//=============================================================================

#include "tooltip_widget.h"

#include <algorithm>

#include <QApplication>
#include <QGuiApplication>
#include <QMainWindow>
#include <QScreen>
#include <QSizePolicy>
#include <QString>
#include <QVBoxLayout>

#include "qt_common/utils/qt_util.h"

namespace
{
    static const int     kMousePositionBuffer = 15;  ///< Position tooltips in front of the mouse.
    static const QString kTooltipStylesheet   = QString("TooltipWidget > QWidget#background_widget_ { border: %1px solid palette(text); }")
                                                  .arg(QString::number(TooltipWidget::kTooltipBorder));  ///< Make border match color theme.
}  // namespace

TooltipWidget::TooltipWidget(QWidget* parent, bool follow_mouse_position, QWidget* container_widget, std::vector<QScrollArea*> container_scroll_areas)
    : QWidget(parent)
    , follow_mouse_position_(follow_mouse_position)
    , background_widget_(nullptr)
    , container_widget_(container_widget)
{
    // Tell the tooltip to paint the background for widgets contained inside it. The tooltip is sometimes transparent otherwise, like on Linux Wayland.
    setAutoFillBackground(true);

    // Containerize the contents of the tooltip in a dummy widget. Style rules will not work otherwise, like on Linux Wayland
    QVBoxLayout* background_layout = new QVBoxLayout;
    background_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(background_layout);

    background_widget_ = new QWidget(this);
    background_widget_->setObjectName("background_widget_");
    background_widget_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    background_layout->addWidget(background_widget_);

    // Track mouse events to assist hiding this tooltip if this gets a mouse event outside of its parents geometry,
    // or if you want to always keep this tooltip in front of the mouse.
    setMouseTracking(true);
    background_widget_->setMouseTracking(true);

    // This widget's size should not change after its contents have been set.
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Only show this widget when the mouse is over something special.
    hide();

    // This widget should not get focus.
    setFocusPolicy(Qt::NoFocus);

    // This widget should behave like a tooltip; Linux Wayland will not let you move widgets with this flag.
    if (!follow_mouse_position_)
    {
        setWindowFlag(Qt::ToolTip, true);
    }
    else
    {
#ifndef Q_OS_LINUX
        setWindowFlag(Qt::ToolTip, true);
#endif
    }

    // This widget should never be the active window; the main window should remain the active window.
    setAttribute(Qt::WA_ShowWithoutActivating);

    // Force refresh the style sheet if the color theme changes.
    connect(&QtCommon::QtUtils::ColorTheme::Get(), &QtCommon::QtUtils::ColorTheme::ColorThemeUpdated, this, [this]() { setStyleSheet(kTooltipStylesheet); });

    setStyleSheet(kTooltipStylesheet);

    if (container_widget_ != nullptr)
    {
        // Install an event filter on this tooltip to assist hiding it when the mouse leaves its container.
        container_widget_->installEventFilter(this);
    }

    RegisterScrollAreas(container_scroll_areas);
#ifdef Q_OS_LINUX
    connect(&hide_timer_, &QTimer::timeout, this, &TooltipWidget::HideTimerCallback);
#endif
}

TooltipWidget::~TooltipWidget()
{
}

void TooltipWidget::UpdatePosition(QPoint new_global_position)
{
    // Check if we need to modify the position so this widget stays visible.
    QRect      clip_geometry;
    bool       clipped          = false;
    const auto tooltip_geometry = frameGeometry();
    const auto tooltip_end_x    = new_global_position.x() + tooltip_geometry.width();
    const auto tooltip_end_y    = new_global_position.y() + tooltip_geometry.height();

#ifdef Q_OS_LINUX
    // On Linux clip the tooltip to the main window, not the screen.
    QMainWindow* main_window = nullptr;

    for (QWidget* widget : QApplication::topLevelWidgets())
    {
        if (widget->inherits("QMainWindow"))
        {
            main_window = qobject_cast<QMainWindow*>(widget);
            break;
        }
    }

    if (main_window != nullptr)
    {
        clip_geometry = main_window->geometry();
    }
#else
    const QScreen* current_screen = QGuiApplication::screenAt(new_global_position);
    if (current_screen != nullptr)
    {
        clip_geometry = current_screen->availableGeometry();
    }
#endif

    const auto clip_end_x = clip_geometry.x() + clip_geometry.width();
    const auto clip_end_y = clip_geometry.y() + clip_geometry.height();

    if (tooltip_end_x > clip_end_x)
    {
        // Cursor too close to the right edge; prevent this widget from being clipped.
        const auto adjusted_x = new_global_position.x() - tooltip_geometry.width();
        new_global_position.setX(adjusted_x);
        clipped = true;

#ifdef Q_OS_LINUX
        // Tooltip is now too close to the left edge so move it to the center.
        if (new_global_position.x() < clip_geometry.x())
        {
            new_global_position.setX(new_global_position.x() + (tooltip_geometry.width() / 2));
        }
#endif
    }

    if (tooltip_end_y > clip_end_y)
    {
        // Cursor too close to the bottom edge; prevent this widget from being clipped.
        const auto adjusted_y = new_global_position.y() - tooltip_geometry.height();
        new_global_position.setY(adjusted_y);

#ifdef Q_OS_LINUX
        // Tooltip is now too close to the top edge so move it to the center.
        if (new_global_position.y() < clip_geometry.y())
        {
            new_global_position.setY(new_global_position.y() + (tooltip_geometry.height() / 2));
        }
#endif
    }

    const auto local_position  = this->mapFromGlobal(new_global_position);
    auto       parent_position = this->mapToParent(local_position);

    parent_position.setX(clipped ? (parent_position.x() - kMousePositionBuffer) : (parent_position.x() + kMousePositionBuffer));

    move(parent_position);
}

bool TooltipWidget::ContainerGeometryContainsPosition(QPoint global_position) const
{
    if (container_widget_ == nullptr)
    {
        return false;
    }

    // Check if the position is inside this tooltip's container's geometry.
    const auto container_local_position   = container_widget_->mapFromGlobal(global_position);
    const auto parent_offset_x            = container_widget_->pos().x();
    const auto parent_offset_y            = container_widget_->pos().y();
    const auto container_geometry         = container_widget_->geometry().adjusted(-parent_offset_x, -parent_offset_y, -parent_offset_x, -parent_offset_y);
    bool       tooltip_still_in_container = container_geometry.contains(container_local_position);

    // Account for any scroll areas which require a separate geometry check.
    for (const auto container_scroll_area : container_scroll_areas_)
    {
        if (container_scroll_area != nullptr)
        {
            const auto scroll_parent_offset_x = container_scroll_area->viewport()->pos().x();
            const auto scroll_parent_offset_y = container_scroll_area->viewport()->pos().y();
            const auto scroll_area_geometry   = container_scroll_area->viewport()->frameGeometry().adjusted(
                -scroll_parent_offset_x, -scroll_parent_offset_y, -scroll_parent_offset_x, -scroll_parent_offset_y);
            const auto scroll_area_position = container_scroll_area->viewport()->mapFromGlobal(global_position);

            tooltip_still_in_container &= scroll_area_geometry.contains(scroll_area_position);
        }
    }

    return tooltip_still_in_container;
}

void TooltipWidget::RegisterScrollAreas(std::vector<QScrollArea*> container_scroll_areas)
{
    for (auto* container_scroll_area : container_scroll_areas)
    {
        if (container_scroll_area != nullptr)
        {
            container_scroll_areas_.push_back(container_scroll_area);

            // Install an event filter on this tooltip to assist hiding it when the mouse leaves its container scroll areas.
            container_scroll_area->installEventFilter(this);
        }
    }
}

void TooltipWidget::leaveEvent(QEvent* event)
{
    QWidget::leaveEvent(event);

    if (!ContainerGeometryContainsPosition(QCursor::pos()))
    {
        hide();
    }
#ifdef Q_OS_LINUX
    else
    {
        hide_timer_.start(kTooltipDelayMs);
    }
#endif
}

#ifdef Q_OS_LINUX
void TooltipWidget::enterEvent(QEnterEvent* event)
{
    QWidget::enterEvent(event);
    hide_timer_.stop();
}
#endif

void TooltipWidget::mouseMoveEvent(QMouseEvent* event)
{
    event->accept();

    if (container_widget_ == nullptr)
    {
        return;
    }

    const auto global_position = QCursor::pos();

    if (follow_mouse_position_ && !ContainerGeometryContainsPosition(global_position))
    {
        // This tooltip is supposed to stay with the mouse, and a move event occurred inside its geometry
        // but outside of the geometry of its container widget so hide this tooltip.
        hide();

        return;
    }

    if (follow_mouse_position_)
    {
        // Nudge this widget just ahead of the mouse position so it does not get stuck.
        UpdatePosition(global_position);
    }
}

bool TooltipWidget::eventFilter(QObject* object, QEvent* event)
{
#ifdef Q_OS_LINUX
    if(object == container_widget_ && event->type() == QEvent::Enter)
    {
        hide_timer_.stop();
    }
#endif

    if (object == container_widget_ && event->type() == QEvent::Leave)
    {
        if (!ContainerGeometryContainsPosition(QCursor::pos()))
        {
            // Hide this tooltip because the mouse has left its container.
            hide();
        }
#ifdef Q_OS_LINUX
        else
        {
            hide_timer_.start(kTooltipDelayMs);
        }
#endif
    }
    else if (object == container_widget_ && event->type() == QEvent::Hide)
    {
        // Hide this tooltip because its container was hidden.
        hide();
    }
    else if (object == container_widget_ && event->type() == QEvent::Destroy)
    {
        // Stop tracking the container if it gets destroyed.
        container_widget_ = nullptr;
    }
    else if (std::find(container_scroll_areas_.begin(), container_scroll_areas_.end(), object) != container_scroll_areas_.end() &&
             event->type() == QEvent::Destroy)
    {
        // Stop tracking any container scroll areas that get destroyed.
        container_scroll_areas_.erase(std::remove(container_scroll_areas_.begin(), container_scroll_areas_.end(), object));
    }

    return false;
}

#ifdef Q_OS_LINUX
void TooltipWidget::HideTimerCallback()
{
    hide_timer_.stop();
    hide();
}
#endif
