//=============================================================================
/// Copyright (c) 2016-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a double slider widget.
//=============================================================================

#include "double_slider_widget.h"

#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QStyleOptionSlider>
#include <QStylePainter>

#include "utils/common_definitions.h"
#include "utils/scaling_manager.h"

/// Stylesheet for the double slider widget.
static const QString kCustomSliderStylesheet(
    "QSlider::groove:horizontal {"
    "height: 2px;"
    "background-color : #AFAFAF;"
    "margin: 2px 0;"
    "}"

    "QSlider::handle:horizontal {"
    "background-color : #0074D6;"
    "width: 6px;"
    "height:15px;"
    "margin: -15px 0;"
    "}"

    "QSlider::handle:horizontal:hover {"
    "background-color : black;"
    "width: 6px;"
    "height:15px;"
    "margin: -15px 0"
    "}"

    "QSlider::handle:disabled {"
    "background-color : grey;"
    "width: 6px;"
    "height:15px;"
    "margin: -15px 0"
    "}");

DoubleSliderWidget::DoubleSliderWidget(QWidget* parent)
    : QSlider(parent)
{
    Init();
}

DoubleSliderWidget::DoubleSliderWidget(Qt::Orientation orientation, QWidget* parent)
    : QSlider(orientation, parent)
{
    Init();
}

void DoubleSliderWidget::Init()
{
    lower_value_           = 0;
    upper_value_           = 0;
    lower_pos_             = 0;
    upper_pos_             = 0;
    offset_pos             = 0;
    position_              = 0;
    last_pressed_span_     = DoubleSliderWidget::kNoHandle;
    main_span_control_     = DoubleSliderWidget::kLowerHandle;
    lower_pressed_control_ = QStyle::SC_None;
    upper_pressed_control_ = QStyle::SC_None;
    handle_movement_       = DoubleSliderWidget::kNoOverlapping;
    is_first_movement_     = false;
    block_tracking_        = false;

    connect(this, &DoubleSliderWidget::rangeChanged, this, &DoubleSliderWidget::UpdateRange);
    connect(this, &DoubleSliderWidget::SliderReleased, this, &DoubleSliderWidget::MovePressedHandle);
    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &QSlider::updateGeometry);

    setStyleSheet(kCustomSliderStylesheet);

    // Sets the upper and lower position to range of a slider.
    SetSpan(minimum(), maximum());
}

DoubleSliderWidget::~DoubleSliderWidget()
{
    disconnect(this, &DoubleSliderWidget::rangeChanged, this, &DoubleSliderWidget::UpdateRange);
    disconnect(this, &DoubleSliderWidget::SliderReleased, this, &DoubleSliderWidget::MovePressedHandle);
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &QSlider::updateGeometry);
}

QSize DoubleSliderWidget::sizeHint() const
{
    return ScalingManager::Get().Scaled(QSlider::sizeHint());
}

void DoubleSliderWidget::InitStyleOption(QStyleOptionSlider* option, DoubleSliderWidget::SpanHandle span_handle) const
{
    QSlider::initStyleOption(option);
    option->sliderPosition = (span_handle == DoubleSliderWidget::kLowerHandle ? lower_pos_ : upper_pos_);
    option->sliderValue    = (span_handle == DoubleSliderWidget::kLowerHandle ? lower_value_ : upper_value_);
}

int DoubleSliderWidget::PixelPosToRangeValue(int pixel_position) const
{
    QStyleOptionSlider option;
    initStyleOption(&option);

    int            slider_min    = 0;
    int            slider_max    = 0;
    int            slider_length = 0;
    const QSlider* slider      = this;
    const QRect    groove_rect   = slider->style()->subControlRect(QStyle::CC_Slider, &option, QStyle::SC_SliderGroove, slider);
    const QRect    handle_rect   = slider->style()->subControlRect(QStyle::CC_Slider, &option, QStyle::SC_SliderHandle, slider);

    if (slider->orientation() == Qt::Horizontal)
    {
        slider_length = handle_rect.width();
        slider_min    = groove_rect.x();
        slider_max    = groove_rect.right() - slider_length + 1;
    }
    else
    {
        slider_length = handle_rect.height();
        slider_min    = groove_rect.y();
        slider_max    = groove_rect.bottom() - slider_length + 1;
    }

    return QStyle::sliderValueFromPosition(slider->minimum(), slider->maximum(), pixel_position - slider_min, slider_max - slider_min, option.upsideDown);
}

void DoubleSliderWidget::HandleMousePress(const QPoint& mouse_position, QStyle::SubControl& control, int value, DoubleSliderWidget::SpanHandle span_hande)
{
    QStyleOptionSlider option;
    InitStyleOption(&option, span_hande);

    const QStyle::SubControl old_control = control;
    control                             = style()->hitTestComplexControl(QStyle::CC_Slider, &option, mouse_position, this);
    const QRect handle_rect              = style()->subControlRect(QStyle::CC_Slider, &option, QStyle::SC_SliderHandle, this);

    if (control == QStyle::SC_SliderHandle)
    {
        position_          = value;
        offset_pos         = Pick(mouse_position - handle_rect.topLeft());
        last_pressed_span_ = span_hande;
        setSliderDown(true);
        emit SliderPressed(span_hande);
    }

    if (control != old_control)
    {
        update(handle_rect);
    }
}

void DoubleSliderWidget::SetupPainter(QPainter* painter, Qt::Orientation orientation, qreal start_x, qreal start_y, qreal end_x, qreal end_y) const
{
    QColor          highlight = palette().color(QPalette::Highlight);
    QLinearGradient gradient(start_x, start_y, end_x, end_y);
    gradient.setColorAt(0, highlight.dark(120));
    gradient.setColorAt(1, highlight.light(108));
    painter->setBrush(gradient);

    if (orientation == Qt::Horizontal)
    {
        painter->setPen(QPen(highlight.dark(130), 0));
    }
    else
    {
        painter->setPen(QPen(highlight.dark(150), 0));
    }
}

void DoubleSliderWidget::DrawSpan(QStylePainter* painter, const QRect& span_area) const
{
    QStyleOptionSlider option;
    InitStyleOption(&option);

    const QSlider* double_slider = this;
    QRect          groove_rect    = double_slider->style()->subControlRect(QStyle::CC_Slider, &option, QStyle::SC_SliderGroove, double_slider);

    if (option.orientation == Qt::Horizontal)
    {
        groove_rect.adjust(0, 2, -1, 0);
    }
    else
    {
        groove_rect.adjust(2, 0, 0, -1);
    }

    groove_rect.setHeight(1);

    // pen & brush
    painter->setPen(QPen(double_slider->palette().color(QPalette::Dark).light(), 0));

    if (option.orientation == Qt::Horizontal)
    {
        SetupPainter(painter, option.orientation, groove_rect.center().x(), groove_rect.top(), groove_rect.center().x(), groove_rect.bottom());
    }
    else
    {
        SetupPainter(painter, option.orientation, groove_rect.left(), groove_rect.center().y(), groove_rect.right(), groove_rect.center().y());
    }

    // draw groove
    painter->drawRect(span_area.intersected(groove_rect));
}

void DoubleSliderWidget::DrawHandle(QStylePainter* painter, DoubleSliderWidget::SpanHandle span_handle) const
{
    QStyleOptionSlider option;
    InitStyleOption(&option, span_handle);
    option.subControls = QStyle::SC_SliderHandle;

    QStyle::SubControl pressed = (span_handle == DoubleSliderWidget::kLowerHandle ? lower_pressed_control_ : upper_pressed_control_);

    if (pressed == QStyle::SC_SliderHandle)
    {
        option.activeSubControls = pressed;
        option.state |= QStyle::State_Sunken;
    }

    painter->drawComplexControl(QStyle::CC_Slider, option);
}

void DoubleSliderWidget::TriggerAction(QAbstractSlider::SliderAction slider_action, bool main_action)
{
    int                                  slider_value    = 0;
    bool                                 no_slider_action = false;
    bool                                 up_slider_action = false;
    const int                            min_value       = minimum();
    const int                            max_value       = maximum();
    const DoubleSliderWidget::SpanHandle alt_control =
        (main_span_control_ == DoubleSliderWidget::kLowerHandle ? DoubleSliderWidget::kUpperHandle : DoubleSliderWidget::kLowerHandle);

    block_tracking_ = true;

    switch (slider_action)
    {
    case QAbstractSlider::SliderSingleStepAdd:
        if ((main_action && main_span_control_ == DoubleSliderWidget::kUpperHandle) || (!main_action && alt_control == DoubleSliderWidget::kUpperHandle))
        {
            slider_value    = qBound(min_value, upper_value_ + singleStep(), max_value);
            up_slider_action = true;
            break;
        }

        slider_value = qBound(min_value, lower_value_ + singleStep(), max_value);
        break;

    case QAbstractSlider::SliderSingleStepSub:
        if ((main_action && main_span_control_ == DoubleSliderWidget::kUpperHandle) || (!main_action && alt_control == DoubleSliderWidget::kUpperHandle))
        {
            slider_value    = qBound(min_value, upper_value_ - singleStep(), max_value);
            up_slider_action = true;
            break;
        }

        slider_value = qBound(min_value, lower_value_ - singleStep(), max_value);
        break;

    case QAbstractSlider::SliderToMinimum:
        slider_value = min_value;

        if ((main_action && main_span_control_ == DoubleSliderWidget::kUpperHandle) || (!main_action && alt_control == DoubleSliderWidget::kUpperHandle))
        {
            up_slider_action = true;
        }

        break;

    case QAbstractSlider::SliderToMaximum:
        slider_value = max_value;

        if ((main_action && main_span_control_ == DoubleSliderWidget::kUpperHandle) || (!main_action && alt_control == DoubleSliderWidget::kUpperHandle))
        {
            up_slider_action = true;
        }

        break;

    case QAbstractSlider::SliderMove:
        if ((main_action && main_span_control_ == DoubleSliderWidget::kUpperHandle) || (!main_action && alt_control == DoubleSliderWidget::kUpperHandle))
        {
            up_slider_action = true;
        }
        no_slider_action = true;
        break;
    case QAbstractSlider::SliderNoAction:
        no_slider_action = true;
        break;

    default:
        qWarning("DoubleSliderWidget::TriggerAction: Unknown action");
        break;
    }

    if (!no_slider_action)
    {
        if (up_slider_action)
        {
            if (handle_movement_ == DoubleSliderWidget::kNoCrossing)
            {
                slider_value = qMax(slider_value, lower_value_);
            }
            else if (handle_movement_ == DoubleSliderWidget::kNoOverlapping)
            {
                slider_value = qMax(slider_value, lower_value_ + 1);
            }

            if (handle_movement_ == DoubleSliderWidget::kFreeMovement && slider_value < lower_value_)
            {
                SwapControls();
                SetLowerPosition(slider_value);
            }
            else
            {
                SetUpperPosition(slider_value);
            }
        }
        else
        {
            if (handle_movement_ == DoubleSliderWidget::kNoCrossing)
            {
                slider_value = qMin(slider_value, upper_value_);
            }
            else if (handle_movement_ == DoubleSliderWidget::kNoOverlapping)
            {
                slider_value = qMin(slider_value, upper_value_ - 1);
            }

            if (handle_movement_ == DoubleSliderWidget::kFreeMovement && slider_value > upper_value_)
            {
                SwapControls();
                SetUpperPosition(slider_value);
            }
            else
            {
                SetLowerPosition(slider_value);
            }
        }
    }

    block_tracking_ = false;
    SetLowerValue(lower_pos_);
    SetUpperValue(upper_pos_);
}

void DoubleSliderWidget::SwapControls()
{
    qSwap(lower_value_, upper_value_);
    qSwap(lower_pressed_control_, upper_pressed_control_);

    last_pressed_span_ = (last_pressed_span_ == DoubleSliderWidget::kLowerHandle ? DoubleSliderWidget::kUpperHandle : DoubleSliderWidget::kLowerHandle);
    main_span_control_ = (main_span_control_ == DoubleSliderWidget::kLowerHandle ? DoubleSliderWidget::kUpperHandle : DoubleSliderWidget::kLowerHandle);
}

void DoubleSliderWidget::UpdateRange(int min_value, int max_value)
{
    Q_UNUSED(min_value);
    Q_UNUSED(max_value);

    // SetSpan() takes care of keeping span in range
    SetSpan(lower_value_, upper_value_);
}

void DoubleSliderWidget::MovePressedHandle()
{
    switch (last_pressed_span_)
    {
    case DoubleSliderWidget::kLowerHandle:
        if (lower_pos_ != lower_value_)
        {
            bool main_action = (main_span_control_ == DoubleSliderWidget::kLowerHandle);
            TriggerAction(QAbstractSlider::SliderMove, main_action);
        }
        break;

    case DoubleSliderWidget::kUpperHandle:
        if (upper_pos_ != upper_value_)
        {
            bool main_action = (main_span_control_ == DoubleSliderWidget::kUpperHandle);
            TriggerAction(QAbstractSlider::SliderMove, main_action);
        }
        break;

    default:
        break;
    }
}

DoubleSliderWidget::HandleMovementModeType DoubleSliderWidget::HandleMovementMode() const
{
    return handle_movement_;
}

void DoubleSliderWidget::SetHandleMovementMode(DoubleSliderWidget::HandleMovementModeType movement_mode)
{
    handle_movement_ = movement_mode;
}

int DoubleSliderWidget::LowerValue() const
{
    return qMin(lower_value_, upper_value_);
}

void DoubleSliderWidget::SetLowerValue(int lower_value)
{
    SetSpan(lower_value, upper_value_);
}

int DoubleSliderWidget::UpperValue() const
{
    return qMax(lower_value_, upper_value_);
}

void DoubleSliderWidget::SetUpperValue(int upper_value)
{
    SetSpan(lower_value_, upper_value);
}

void DoubleSliderWidget::SetSpan(int lower_value, int upper_value)
{
    const int lower_handle_value = qBound(minimum(), qMin(lower_value, upper_value), maximum());
    const int upper_handle_value = qBound(minimum(), qMax(lower_value, upper_value), maximum());

    if (lower_handle_value != lower_value_ || upper_handle_value != upper_value_)
    {
        if (lower_handle_value != lower_value_)
        {
            lower_value_ = lower_handle_value;
            lower_pos_   = lower_handle_value;
            emit LowerValueChanged(lower_handle_value);
        }

        if (upper_handle_value != upper_value_)
        {
            upper_value_ = upper_handle_value;
            upper_pos_   = upper_handle_value;
            emit UpperValueChanged(upper_handle_value);
        }

        emit SpanChanged(lower_value_, upper_value_);
        update();
    }
}

int DoubleSliderWidget::LowerPosition() const
{
    return lower_pos_;
}

void DoubleSliderWidget::SetLowerPosition(int lower_position)
{
    if (lower_pos_ != lower_position)
    {
        lower_pos_ = lower_position;

        if (!hasTracking())
        {
            update();
        }

        if (isSliderDown())
        {
            emit LowerPositionChanged(lower_position);
        }

        if (hasTracking() && !block_tracking_)
        {
            bool main_action = (main_span_control_ == DoubleSliderWidget::kLowerHandle);
            TriggerAction(SliderMove, main_action);
        }
    }
}

int DoubleSliderWidget::UpperPosition() const
{
    return upper_pos_;
}

void DoubleSliderWidget::SetUpperPosition(int upper_position)
{
    if (upper_pos_ != upper_position)
    {
        upper_pos_ = upper_position;

        if (!hasTracking())
        {
            update();
        }

        if (isSliderDown())
        {
            emit UpperPositionChanged(upper_position);
        }

        if (hasTracking() && !block_tracking_)
        {
            bool main_action = (main_span_control_ == DoubleSliderWidget::kUpperHandle);
            TriggerAction(SliderMove, main_action);
        }
    }
}

int DoubleSliderWidget::Pick(const QPoint& handle_point) const
{
    return orientation() == Qt::Horizontal ? handle_point.x() : handle_point.y();
}

void DoubleSliderWidget::keyPressEvent(QKeyEvent* event)
{
    QSlider::keyPressEvent(event);

    bool         main_action   = true;
    SliderAction slider_action = SliderNoAction;

    switch (event->key())
    {
    case Qt::Key_Left:
        main_action   = (orientation() == Qt::Horizontal);
        slider_action = !invertedAppearance() ? SliderSingleStepSub : SliderSingleStepAdd;
        break;

    case Qt::Key_Right:
        main_action   = (orientation() == Qt::Horizontal);
        slider_action = !invertedAppearance() ? SliderSingleStepAdd : SliderSingleStepSub;
        break;

    case Qt::Key_Up:
        main_action   = (orientation() == Qt::Vertical);
        slider_action = invertedControls() ? SliderSingleStepSub : SliderSingleStepAdd;
        break;

    case Qt::Key_Down:
        main_action   = (orientation() == Qt::Vertical);
        slider_action = invertedControls() ? SliderSingleStepAdd : SliderSingleStepSub;
        break;

    case Qt::Key_Home:
        main_action   = (main_span_control_ == DoubleSliderWidget::kLowerHandle);
        slider_action = SliderToMinimum;
        break;

    case Qt::Key_End:
        main_action   = (main_span_control_ == DoubleSliderWidget::kUpperHandle);
        slider_action = SliderToMaximum;
        break;

    default:
        event->ignore();
        break;
    }

    if (slider_action != SliderNoAction)
    {
        TriggerAction(slider_action, main_action);
    }
}

void DoubleSliderWidget::mousePressEvent(QMouseEvent* event)
{
    if (minimum() == maximum() || (event->buttons() ^ event->button()))
    {
        event->ignore();
        return;
    }

    HandleMousePress(event->pos(), upper_pressed_control_, upper_value_, DoubleSliderWidget::kUpperHandle);

    if (upper_pressed_control_ != QStyle::SC_SliderHandle)
    {
        HandleMousePress(event->pos(), lower_pressed_control_, lower_value_, DoubleSliderWidget::kLowerHandle);
    }

    is_first_movement_ = true;
    event->accept();
}

void DoubleSliderWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (lower_pressed_control_ != QStyle::SC_SliderHandle && upper_pressed_control_ != QStyle::SC_SliderHandle)
    {
        event->ignore();
        return;
    }

    QStyleOptionSlider option;
    initStyleOption(&option);
    const int pixel_metric = style()->pixelMetric(QStyle::PM_MaximumDragDistance, &option, this);
    int       new_position = PixelPosToRangeValue(Pick(event->pos()) - offset_pos);

    if (pixel_metric >= 0)
    {
        const QRect slider_rect = rect().adjusted(-pixel_metric, -pixel_metric, pixel_metric, pixel_metric);

        if (!slider_rect.contains(event->pos()))
        {
            new_position = position_;
        }
    }

    // Pick the preferred handle on the first movement
    if (is_first_movement_)
    {
        if (lower_value_ == upper_value_)
        {
            if (new_position < LowerValue())
            {
                SwapControls();
                is_first_movement_ = false;
            }
        }
        else
        {
            is_first_movement_ = false;
        }
    }

    if (lower_pressed_control_ == QStyle::SC_SliderHandle)
    {
        if (handle_movement_ == kNoCrossing)
        {
            new_position = qMin(new_position, UpperValue());
        }
        else if (handle_movement_ == kNoOverlapping)
        {
            new_position = qMin(new_position, UpperValue() - 1);
        }

        if (handle_movement_ == kFreeMovement && new_position > upper_value_)
        {
            SwapControls();
            SetUpperPosition(new_position);
        }
        else
        {
            SetLowerPosition(new_position);
        }
    }
    else if (upper_pressed_control_ == QStyle::SC_SliderHandle)
    {
        if (handle_movement_ == kNoCrossing)
        {
            new_position = qMax(new_position, LowerValue());
        }
        else if (handle_movement_ == kNoOverlapping)
        {
            new_position = qMax(new_position, LowerValue() + 1);
        }

        if (handle_movement_ == kFreeMovement && new_position < lower_value_)
        {
            SwapControls();
            SetLowerPosition(new_position);
        }
        else
        {
            SetUpperPosition(new_position);
        }
    }

    event->accept();
}

void DoubleSliderWidget::mouseReleaseEvent(QMouseEvent* event)
{
    QSlider::mouseReleaseEvent(event);
    setSliderDown(false);
    lower_pressed_control_ = QStyle::SC_None;
    upper_pressed_control_ = QStyle::SC_None;
    emit SliderReleased();
    update();
}

void DoubleSliderWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QStylePainter painter(this);

    // groove & ticks
    QStyleOptionSlider option;
    initStyleOption(&option);
    option.sliderValue    = 0;
    option.sliderPosition = 0;
    option.subControls    = QStyle::SC_SliderGroove | QStyle::SC_SliderTickmarks;
    painter.drawComplexControl(QStyle::CC_Slider, option);

    // handle rects
    option.sliderPosition           = lower_pos_;
    const QRect lower_handle_rect  = style()->subControlRect(QStyle::CC_Slider, &option, QStyle::SC_SliderHandle, this);
    const int   lower_handle_value = Pick(lower_handle_rect.center());
    option.sliderPosition           = upper_pos_;
    const QRect upper_handle_rect  = style()->subControlRect(QStyle::CC_Slider, &option, QStyle::SC_SliderHandle, this);
    const int   upper_handle_value = Pick(upper_handle_rect.center());

    // span
    const int    min_value    = qMin(lower_handle_value, upper_handle_value);
    const int    max_value    = qMax(lower_handle_value, upper_handle_value);
    const QPoint center_point = QRect(lower_handle_rect.center(), upper_handle_rect.center()).center();
    QRect        span_rect;

    if (orientation() == Qt::Horizontal)
    {
        span_rect = QRect(QPoint(min_value, center_point.y() - 2), QPoint(max_value, center_point.y() + 1));
    }
    else
    {
        span_rect = QRect(QPoint(center_point.x() - 2, min_value), QPoint(center_point.x() + 1, max_value));
    }

    DrawSpan(&painter, span_rect);

    // handles
    switch (last_pressed_span_)
    {
    case DoubleSliderWidget::kLowerHandle:
        DrawHandle(&painter, DoubleSliderWidget::kUpperHandle);
        DrawHandle(&painter, DoubleSliderWidget::kLowerHandle);
        break;

    case DoubleSliderWidget::kUpperHandle:
    default:
        DrawHandle(&painter, DoubleSliderWidget::kLowerHandle);
        DrawHandle(&painter, DoubleSliderWidget::kUpperHandle);
        break;
    }
}
