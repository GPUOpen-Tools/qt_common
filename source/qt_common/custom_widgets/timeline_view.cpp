//=============================================================================
/// Copyright (c) 2018-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a timeline
//=============================================================================

#include "timeline_view.h"

#include <QAbstractScrollArea>
#include <QApplication>
#include <QDebug>
#include <QMouseEvent>
#include <QScrollBar>
#include <QStyle>

#include "common_definitions.h"
#include "scaling_manager.h"

static const QColor  kSelectionColor            = QColor(0, 118, 215);
static const quint64 kZoomToSelectionPercentage = 10;

TimelineView::TimelineView(QWidget* pParent)
    : QGraphicsView(pParent)
    , ruler_(nullptr)
    , mouse_indicator_(nullptr)
    , selection_box_(nullptr)
    , scene_(nullptr)
    , is_selection_defined_(false)
    , is_selection_held_(false)
    , selection_pos_x_(0)
    , selected_start_clock_(0)
    , selected_end_clock_(0)
    , viewable_start_clock_(0)
    , viewable_end_clock_(0)
    , last_hovered_clock_(0)
    , ruler_zoom_factor_(1.0f)
    , left_scene_bound_(0)
    , right_scene_bound_(0)
    , in_reset_state_(true)
{
    setMouseTracking(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    verticalScrollBar()->blockSignals(true);

    scene_ = new QGraphicsScene();
    setScene(scene_);

    ruler_config_ = RulerConfig();

    ruler_config_.max_time            = 10000;
    ruler_config_.height              = kDefaultRulerHeight;
    ruler_config_.width               = this->width();
    ruler_config_.time_to_clock_ratio = 1;
    ruler_config_.unit_type           = kTimeUnitTypeClk;

    ruler_ = new RulerWidget(ruler_config_);
    ruler_->setPos(0, 0);

    mouse_indicator_ = new QGraphicsLineItem();
    mouse_indicator_->setZValue(1);

    auto mouse_pen = mouse_indicator_->pen();
    mouse_pen.setColor(Qt::gray);
    mouse_pen.setCosmetic(true);  // Don't scale.
    mouse_indicator_->setPen(mouse_pen);

    selection_box_ = new QGraphicsRectItem();
    selection_box_->setBrush(kSelectionColor);
    selection_box_->setPen(Qt::NoPen);
    selection_box_->setOpacity(0.0);
    selection_box_->setZValue(1);

    scene_->addItem(ruler_);
    scene_->addItem(mouse_indicator_);
    scene_->addItem(selection_box_);

    UpdateScene();

    connect(horizontalScrollBar(), &QScrollBar::valueChanged, this, &TimelineView::ScrollBarChanged);
}

TimelineView::~TimelineView()
{
    delete mouse_indicator_;
    delete selection_box_;
    delete ruler_;
}

void TimelineView::UpdateSceneBounds()
{
    const QRectF scene_rect = QRectF(0, 0, ruler_config_.width, BasePosY());
    scene_->setSceneRect(scene_rect);

    left_scene_bound_  = mapToScene(QPoint(0, 0)).x();
    right_scene_bound_ = mapToScene(QPoint(width(), 0)).x();

    if (in_reset_state_)
    {
        viewable_start_clock_ = 0;
        viewable_end_clock_   = ruler_config_.max_time;
    }
    else
    {
        viewable_start_clock_ = SceneCoordinateToClock(left_scene_bound_);

        viewable_end_clock_ = SceneCoordinateToClock(right_scene_bound_);

        ClampClocks(viewable_start_clock_, viewable_end_clock_);
    }

    ruler_->UpdateVisibleBounds(viewable_start_clock_, viewable_end_clock_);
}

int TimelineView::ClockToSceneCoordinate(qint64 clock) const
{
    return clock * ((double)ruler_config_.width / (double)ruler_config_.max_time);
}

qint64 TimelineView::SceneCoordinateToClock(int sceneCoordX) const
{
    return (sceneCoordX) * ((double)ruler_config_.max_time / (double)ruler_config_.width);
}

void TimelineView::UpdateRuler()
{
    ruler_config_.max_time = ((qint64)ruler_config_.max_time > width()) ? ruler_config_.max_time : width();

    ruler_->Update(ruler_config_);

    // Scene bounds is driven by ruler width, so update
    UpdateSceneBounds();

    // Move ruler to top left
    //const QPointF pos = QPointF((ruler_config_.width / 2) * -1, (height() / 2) * -1 + ruler_config_.height / 2);
    //ruler_->setPos(pos);
}

void TimelineView::SetMaxClock(qint64 clock)
{
    ruler_config_.max_time = clock;

    UpdateScene();
}

bool TimelineView::GetResetState() const
{
    return in_reset_state_;
}

void TimelineView::ScrollBarChanged()
{
    UpdateScene();
}

void TimelineView::UpdateMouseIndicator()
{
    const auto x_position = ClockToSceneCoordinate(last_hovered_clock_);

    if (is_ruler_hidden_ == true)
    {
        mouse_indicator_->setLine(x_position, 0, x_position, height());
    }
    else
    {
        mouse_indicator_->setLine(x_position, kDefaultRulerHeight + 1, x_position, height());
    }
}

void TimelineView::UpdateSelectionBox()
{
    const int scene_start_pos_x = ClockToSceneCoordinate(selected_start_clock_);
    const int scene_end_pos_x   = ClockToSceneCoordinate(selected_end_clock_);

    QRectF rect = selection_box_->rect();
    rect.setX(scene_start_pos_x);

    if (is_ruler_hidden_ == true)
    {
        rect.setY(0);
    }
    else
    {
        rect.setY(kDefaultRulerHeight + 1);
    }

    rect.setWidth(scene_end_pos_x - scene_start_pos_x);
    rect.setHeight(height());
    selection_box_->setRect(rect);

    if (is_selection_defined_ == true)
    {
        selection_box_->setOpacity(0.5);
    }
}

void TimelineView::UpdateContent()
{
}

void TimelineView::UpdateScene()
{
    UpdateRuler();
    UpdateMouseIndicator();
    UpdateSelectionBox();
    UpdateContent();

    viewport()->update();
}

void TimelineView::mousePressEvent(QMouseEvent* event)
{
    QGraphicsView::mousePressEvent(event);

    if (event->button() != Qt::RightButton)
    {
        const QPoint  mouse_hover_coords = mapFromGlobal(QCursor::pos());
        const QPointF scene_hover_coords = mapToScene(mouse_hover_coords);

        selection_pos_x_ = scene_hover_coords.x();

        is_selection_defined_ = false;

        selection_box_->setOpacity(0);

        is_selection_held_ = true;

        selected_start_clock_ = 0;
        selected_end_clock_   = 0;
    }
}

void TimelineView::mouseReleaseEvent(QMouseEvent* event)
{
    QGraphicsView::mouseReleaseEvent(event);

    if (event->button() != Qt::RightButton)
    {
        is_selection_held_ = false;
    }
}

void TimelineView::mouseMoveEvent(QMouseEvent* event)
{
    QGraphicsView::mouseMoveEvent(event);

    const QPoint  mouse_hover_coords = mapFromGlobal(QCursor::pos());
    const QPointF scene_hover_coords = mapToScene(mouse_hover_coords);

    qint64 potential_hovered_clock = SceneCoordinateToClock(scene_hover_coords.x());

    if ((potential_hovered_clock >= 0) && (potential_hovered_clock <= (qint64)ruler_config_.max_time))
    {
        last_hovered_clock_ = potential_hovered_clock;
    }

    if (event->button() != Qt::RightButton)
    {
        if (is_selection_held_ == true)
        {
            QRectF rect = selection_box_->rect();

            const int futureDelta = scene_hover_coords.x() - selection_pos_x_;

            if (futureDelta >= 0)
            {
                rect.setX(selection_pos_x_);
                rect.setWidth(futureDelta);
            }
            else
            {
                rect.setX(selection_pos_x_ + futureDelta);
                rect.setWidth(futureDelta * -1);
            }

            selection_box_->setRect(rect);

            selection_box_->setOpacity(0.5);

            is_selection_defined_ = true;

            selected_start_clock_ = SceneCoordinateToClock(rect.x());

            selected_end_clock_ = SceneCoordinateToClock(rect.x() + rect.width());

            ClampClocks(selected_start_clock_, selected_end_clock_);
        }
    }

    UpdateMouseIndicator();

    // May enable for debugging
#if 0
    QPoint mouse_coords = mapFromGlobal(QCursor::pos());
    QPointF scene_coords = mapToScene(mouse_coords);
    qDebug() << "mouse_coords: " << mouse_coords;
    qDebug() << "scene_coords: " << scene_coords;
    qDebug() << "---------";
#endif
}

void TimelineView::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);

    if (in_reset_state_ == true)
    {
        ruler_config_.width = MinimumRulerWidth();
    }
    else
    {
        if (ruler_->boundingRect().width() < width())
        {
            SetResetState();
        }
    }

    UpdateScene();
}

bool TimelineView::ZoomInMousePosition()
{
    return ZoomInMousePositionClk(last_hovered_clock_, 2);
}

bool TimelineView::ZoomInSelection()
{
    if (is_selection_defined_ == true)
    {
        quint64 midpoint   = (selected_start_clock_ + selected_end_clock_) / 2;
        quint64 range      = ((selected_end_clock_ - selected_start_clock_) * (kZoomToSelectionPercentage + 100)) / 100;
        quint64 half_range = range / 2;

        return ZoomInClk(midpoint - half_range, midpoint + half_range);
    }

    return false;
}

bool TimelineView::ZoomInMousePositionClk(qint64 last_hovered_clk, int zoom_rate)
{
    const quint64 left_ratio  = (last_hovered_clk - viewable_start_clock_) / zoom_rate;
    const quint64 right_ratio = (viewable_end_clock_ - last_hovered_clk) / zoom_rate;

    const qint64 begin_clk = last_hovered_clk - left_ratio;
    const qint64 end_clk   = last_hovered_clk + right_ratio;

    return ZoomInClk(begin_clk, end_clk);
}

bool TimelineView::ZoomIn(int zoom_rate, bool use_mouse_position)
{
    if (use_mouse_position == false)
    {
        const qint64 desired_clock_range     = (viewable_end_clock_ - viewable_start_clock_) / zoom_rate;
        const qint64 desired_clock_mid_point = (viewable_end_clock_ + viewable_start_clock_) / 2;
        const qint64 desired_left_clock      = desired_clock_mid_point - (desired_clock_range / 2);
        const qint64 desired_right_clock     = desired_left_clock + desired_clock_range;

        return ZoomInClk(desired_left_clock, desired_right_clock);
    }
    else
    {
        return ZoomInMousePositionClk(last_hovered_clock_, zoom_rate);
    }
}

void TimelineView::SetScrollbarAtClock(qint64 begin_clk)
{
    QScrollBar* graphics_scroll_bar = horizontalScrollBar();

    const double clock_percentage    = (double)begin_clk / (double)ruler_config_.max_time;
    const int    scroll_bar_offset   = graphics_scroll_bar->pageStep() * clock_percentage * ruler_zoom_factor_;
    const int    scoll_bar_new_value = graphics_scroll_bar->minimum() + scroll_bar_offset;

    graphics_scroll_bar->setValue(scoll_bar_new_value);
}

bool TimelineView::ZoomInClk(qint64 begin_clk, qint64 end_clk)
{
    // Reset the view before zooming to preserve accuracy.
    ruler_config_.width = MinimumRulerWidth();
    ruler_zoom_factor_  = 1.0f;
    UpdateScene();

    qint64 selected_clock_range = end_clk - begin_clk;

    // if the selected range is too small, zoom in as far as possible
    if (selected_clock_range < width())
    {
        qint64 half_width = (width() - selected_clock_range) / 2;
        begin_clk -= half_width;
        end_clk += half_width;
    }

    // limit zoom level based on resolution of the scene / scroll bar
    uint64_t minimum_range = (ruler_config_.max_time * width()) / selected_clock_range;
    if (minimum_range > INT_MAX)
    {
        // zoomed in too far for integer precision, so back out as far as possible
        uint64_t mid_point  = (end_clk + begin_clk) / 2;
        uint64_t max_range  = (ruler_config_.max_time * (width()) / INT_MAX);
        int      half_range = max_range / 2;
        begin_clk           = mid_point - half_range;
        end_clk             = mid_point + half_range;
    }

    bool can_zoom_further = false;

    // Clamp to within range
    ClampClocks(begin_clk, end_clk);

    // Recompute the range in case of clamps
    selected_clock_range = end_clk - begin_clk;

    // Compute a factor to scale the ruler by
    const qint64 visible_clock_range = viewable_end_clock_ - viewable_start_clock_;
    const double zoom_factor         = (double)visible_clock_range / (double)selected_clock_range;

    ruler_zoom_factor_ *= zoom_factor;

    // Scale the ruler and update everything
    ruler_config_.width *= zoom_factor;

    in_reset_state_ = false;

    UpdateScene();

    SetScrollbarAtClock(begin_clk);

    // check if further zooming is possible
    qint64 half_viewable_length = (viewable_end_clock_ - viewable_start_clock_) / 2;
    if (half_viewable_length > width() && minimum_range < INT_MAX)
    {
        can_zoom_further = true;
    }

    return can_zoom_further;
}

bool TimelineView::ZoomOutMousePosition()
{
    return ZoomOutMousePositionClk(last_hovered_clock_, 2);
}

bool TimelineView::ZoomOutMousePositionClk(qint64 last_hovered_clk, int zoom_rate)
{
    const quint64 left_ratio  = last_hovered_clk - viewable_start_clock_;
    const quint64 right_ratio = viewable_end_clock_ - last_hovered_clk;

    const qint64 begin_clk = last_hovered_clk - (left_ratio * zoom_rate);
    const qint64 end_clk   = last_hovered_clk + (right_ratio * zoom_rate);

    return ZoomOutClk(begin_clk, end_clk, zoom_rate);
}

void TimelineView::HideRuler()
{
    if (scene() != nullptr)
    {
        // Check for item in scene before attempting
        // to remove
        if (scene()->items().contains(ruler_))
        {
            scene()->removeItem(ruler_);
            is_ruler_hidden_ = true;
        }
    }
}

void TimelineView::ShowRuler()
{
    if (scene() != nullptr)
    {
        // Check for item already existing, if so ignore
        // request
        if (!scene()->items().contains(ruler_))
        {
            scene()->addItem(ruler_);
            is_ruler_hidden_ = false;
        }
    }
}

void TimelineView::SetResetState()
{
    ruler_config_.width   = MinimumRulerWidth();
    ruler_zoom_factor_    = 1.0f;
    in_reset_state_       = true;
    is_selection_defined_ = false;
    selected_start_clock_ = 0;
    selected_end_clock_   = 0;
}

void TimelineView::ClampClocks(qint64& beginClk, qint64& endClk)
{
    if (beginClk < 0)
    {
        beginClk = 0;

        if (endClk < width())
        {
            endClk = width();
        }
    }
    if (endClk > (qint64)ruler_config_.max_time)
    {
        endClk = ruler_config_.max_time;

        if ((endClk - beginClk) < width())
        {
            beginClk = endClk - width();
        }
    }
}

bool TimelineView::ZoomOut(int zoomRate, bool useMousePos)
{
    if (useMousePos == false)
    {
        const qint64 selected_clock_range = viewable_end_clock_ - viewable_start_clock_;
        const qint64 offset               = (selected_clock_range * (zoomRate - 1)) / 2;
        qint64       begin_clk            = viewable_start_clock_ - offset;
        qint64       end_clk              = viewable_end_clock_ + offset;

        return ZoomOutClk(begin_clk, end_clk, zoomRate);
    }
    else
    {
        return ZoomOutMousePositionClk(last_hovered_clock_, zoomRate);
    }
}

bool TimelineView::ZoomOutClk(qint64 begin_clk, qint64 end_clk, int zoom_rate)
{
    in_reset_state_ = false;

    int potential_width = ruler_config_.width / zoom_rate;

    if (potential_width > MinimumRulerWidth())
    {
        ruler_zoom_factor_ /= zoom_rate;

        // Clamp to within range
        ClampClocks(begin_clk, end_clk);

        ruler_config_.width = potential_width;

        UpdateScene();

        SetScrollbarAtClock(begin_clk);

        return true;
    }
    else
    {
        SetResetState();

        UpdateScene();

        return false;
    }
}

void TimelineView::ZoomReset()
{
    SetResetState();

    UpdateScene();
}

qint64 TimelineView::ViewableStartClk() const
{
    return viewable_start_clock_;
}

qint64 TimelineView::ViewableEndClk() const
{
    return viewable_end_clock_;
}

qint64 TimelineView::SelectedStartClk() const
{
    return selected_start_clock_;
}

qint64 TimelineView::SelectedEndClk() const
{
    return selected_end_clock_;
}

qint64 TimelineView::HoverClk() const
{
    return last_hovered_clock_;
}

int TimelineView::LeftSceneBound() const
{
    return left_scene_bound_;
}

int TimelineView::RightSceneBound() const
{
    return right_scene_bound_;
}

bool TimelineView::RegionSelected() const
{
    return is_selection_defined_;
}

int TimelineView::MinimumRulerWidth() const
{
    return (width() - 2);
}

int TimelineView::BasePosY() const
{
    if (horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOn || horizontalScrollBarPolicy() == Qt::ScrollBarAsNeeded)
    {
        return height() - qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent) - 3;
    }
    else
    {
        return height();
    }
}
