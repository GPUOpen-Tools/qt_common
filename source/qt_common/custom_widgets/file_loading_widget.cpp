//=============================================================================
/// Copyright (c) 2017-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a file loading widget.
//=============================================================================

#include "file_loading_widget.h"

// use definition of PI from math.h
#define _USE_MATH_DEFINES

#include <math.h>
#include <QDebug>
#include <QPaintEvent>
#include <QPainter>

#include "scaling_manager.h"

/// Speed of the animation.
/// Kept in source file to avoid poluting header with <math.h>
static const double kAnimationSpeed = M_PI / 70.0;

/// Class to encapsulate a single animating loading bar
class AnimatedBar
{
public:
    /// Explicit constructor
    /// \param x_pos The x position of the bar.
    /// \param y_pos The y position of the bar.
    /// \param width The width of the bar.
    /// \param height The maximum height of the bar.
    /// \param initial_phase The initial phase of the bar.
    explicit AnimatedBar(int x_pos, int y_pos, int width, int height, double initial_phase)
        : x_pos_(x_pos)
        , y_pos_(y_pos)
        , width_(width)
        , max_height_(height / 2)
        , min_height_(max_height_ * .45)
        , phase_(initial_phase)
        , current_bar_height_(0)
    {
    }

    /// Destructor
    ~AnimatedBar()
    {
    }

    /// Update a single loading bar. Use the absolute value of a sine wave for the
    /// bar height offset. A smoother method would be to use a sine squared wave.
    void Update()
    {
        current_bar_height_ = fabs(sin(phase_));
        current_bar_height_ *= (max_height_ - min_height_);
        phase_ -= kAnimationSpeed;
    }

    /// Reposition and resize the maximum dimensions of this bar.
    /// \param x_pos The new x coordinate.
    /// \param y_pos The new y coordinate.
    /// \param width The new width.
    /// \param height The new maximum height.
    void SetGeometry(const int x_pos, const int y_pos, const int width, const int height)
    {
        x_pos_      = x_pos;
        y_pos_      = y_pos;
        width_      = width;
        max_height_ = height / 2;
        min_height_ = (float)max_height_ * .45f;
    }

    /// Paint a single animation bar
    /// \param painter Pointer to a QPainter object
    void Paint(QPainter* painter)
    {
        qreal x_pos  = x_pos_;
        qreal y_pos  = y_pos_ - max_height_ + current_bar_height_;
        qreal width  = width_;
        qreal height = (max_height_ - current_bar_height_) * 2.0;
        painter->fillRect(QRectF(x_pos, y_pos, width, height), painter->brush());
    }

private:
    int x_pos_;       ///< The animating bar x position
    int y_pos_;       ///< The animating bar y position
    int width_;       ///< Width of the bar
    int max_height_;  ///< Maximum height of the bar above the origin
    int min_height_;  ///< Minimum height of the bar above the origin

    double phase_;               ///< The animating bar phase (how far it is from 0 degrees)
    double current_bar_height_;  ///< The animating bar height
};

/// Class to encapsulate a series of animated loading bars.
class AnimatedBars
{
public:
    /// Constructor
    /// \param width The total width of the bars.
    /// \param height The maximum height.
    /// \param spacing The space in pixels between the bars at 100% DPI scale.
    /// \param num_bars The number of bars to include in the animation.
    AnimatedBars(const int width, const int height, const int spacing, const int num_bars)
    {
        num_bars_ = std::max(1, num_bars);
        bars_.resize(num_bars_);

        SetSize(width, height, spacing);
    }

    /// Destructor
    ~AnimatedBars()
    {
        for (int loop = 0; loop < num_bars_; loop++)
        {
            delete bars_[loop];
        }
    }

    /// Set the maximum extents of the animated bars.
    /// \param width The new width.
    /// \param height The new maximum height.
    /// \param spacing The new spacing between the bars.
    void SetSize(const int width, const int height, const int spacing)
    {
        width_   = std::max(0, width);
        height_  = std::max(0, height);
        spacing_ = std::max(0, std::min(spacing, width));

        // Calculate bar thickness
        const int total_spacing = spacing_ * std::max(0, num_bars_ - 1);
        int       bar_width     = std::max(0, (width_ - total_spacing) / num_bars_);

        int x_pos = 0;
        int y_pos = 0;
        for (int loop = 0; loop < num_bars_; ++loop)
        {
            if (bars_[loop] == nullptr)
            {
                bars_[loop] = new AnimatedBar(x_pos, y_pos, bar_width, height_, (M_PI * (loop - (num_bars_ / 2))) / 8.0);
            }
            else
            {
                bars_[loop]->SetGeometry(x_pos, y_pos, bar_width, height_);
            }
            x_pos += bar_width + spacing_;
        }
    }

    /// Update a series of loading bars.
    void Update()
    {
        for (int loop = 0; loop < num_bars_; ++loop)
        {
            bars_[loop]->Update();
        }
    }

    /// Paint the animation bar series.
    /// \param painter Pointer to a QPainter object.
    void Paint(QPainter* painter)
    {
        for (int loop = 0; loop < num_bars_; ++loop)
        {
            bars_[loop]->Paint(painter);
        }
    }

private:
    int width_;    ///< Total width of the animated bars.
    int height_;   ///< Max height of the animated bars.
    int spacing_;  ///< Spacing between the animated bars.

    int                       num_bars_;  ///< Number of animated bars.
    std::vector<AnimatedBar*> bars_;      ///< Vector of animated bars.

    /// Private constructor cannot be used.
    AnimatedBars(){};

    /// Private constructor cannot be used.
    AnimatedBars(const AnimatedBars&){};
};

FileLoadingWidget::FileLoadingWidget(QWidget* parent)
    : QWidget(parent)
    , animation_timer_(nullptr)
{
    animation_timer_ = new QTimer(this);
    connect(animation_timer_, &QTimer::timeout, this, &FileLoadingWidget::Animate);
    animation_timer_->start(kAnimationTimer_);
    animated_bars_ = new AnimatedBars(contentsRect().width(), contentsRect().height(), kBarHorizontalSpacing_, kNumBars_);

    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &QWidget::updateGeometry);
}

FileLoadingWidget::~FileLoadingWidget()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &QWidget::updateGeometry);
    disconnect(animation_timer_, &QTimer::timeout, this, &FileLoadingWidget::Animate);
    animation_timer_->stop();
    delete animation_timer_;
    delete animated_bars_;
}

QSize FileLoadingWidget::sizeHint() const
{
    QSize size_hint(200, 200);
    return size_hint;
}

void FileLoadingWidget::resizeEvent(QResizeEvent* event)
{
    // Recreate Animation bars with the new size.
    // Temporarily disconnect and stop the timer while the bars gets recreated.
    if (animated_bars_ != nullptr)
    {
        disconnect(animation_timer_, &QTimer::timeout, this, &FileLoadingWidget::Animate);
        animation_timer_->stop();
        animated_bars_->SetSize(contentsRect().width(), contentsRect().height(), kBarHorizontalSpacing_);
        connect(animation_timer_, &QTimer::timeout, this, &FileLoadingWidget::Animate);
        animation_timer_->start(kAnimationTimer_);
    }

    QWidget::resizeEvent(event);
    update();
}

void FileLoadingWidget::Animate()
{
    if (animated_bars_ != nullptr)
    {
        animated_bars_->Update();
        update();
    }
}

void FileLoadingWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw background of widget
    painter.setBrush(palette().window());
    painter.setPen(Qt::NoPen);
    painter.drawRect(0, 0, width(), height());

    painter.setBrush(palette().windowText());
    double x_offset = (contentsRect().width() / 2) - 1;
    painter.translate(contentsRect().center() - QPoint(x_offset, 0));

    animated_bars_->Paint(&painter);
}
