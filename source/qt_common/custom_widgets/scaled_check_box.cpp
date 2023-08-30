//=============================================================================
/// Copyright (c) 2019-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for ScaledCheckBox objects.
//=============================================================================
#include "scaled_check_box.h"

#include <cmath>

#include <QStyle>
#include <QEvent>

#include "qt_util.h"
#include "scaling_manager.h"

ScaledCheckBox::ScaledCheckBox(QWidget* parent)
    : ScaledCheckBox({}, parent)
{
}

ScaledCheckBox::ScaledCheckBox(const QString& text, QWidget* parent)
    : QCheckBox(text, parent)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    // Use a queued connection so that the scaling logic runs on the next main loop iteration
    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledCheckBox::OnScaleFactorChanged, Qt::QueuedConnection);
}

ScaledCheckBox::~ScaledCheckBox()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledCheckBox::OnScaleFactorChanged);
}

QSize ScaledCheckBox::sizeHint() const
{
    ensurePolished();

    int icon_width  = style()->pixelMetric(QStyle::PM_IndicatorWidth, nullptr, this);
    int icon_height = style()->pixelMetric(QStyle::PM_IndicatorHeight, nullptr, this);
    int spacing     = style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, nullptr, this);

    // Calculate length of the text string,
    // using TextShowMnemonic so that keyboard shortcuts indicated by '&' are taken into account.
    const QRect& rect       = fontMetrics().boundingRect(text());
    int          text_width = fontMetrics().boundingRect(rect, Qt::TextShowMnemonic, text()).width();

    QSize size;
    size.setWidth(icon_width + spacing + text_width);
    size.setHeight(std::max(icon_height, fontMetrics().height()));

    return size;
}

void ScaledCheckBox::UpdateIndicatorSize()
{
    // If we naively scale the check box indicator to the same size as the font, we might choose a scale factor like 1.357442.
    // Depending on the size of the font and intrinsic size of the indicator, this can result in the icon becoming distorted.
    // To avoid this, we only scale the indicator by N or 1/N where N is a positive number.
    //
    // This does result in a slight mismatch between the font size and the check indicator, which
    // is the trade off. However, this difference is usually fairly small and most likely won't be noticed,
    // however an improperly scaled image will look far worse.
    //
    // We could attempt to change the icon as the check box scales, but that would require creating icons for every single
    // operating system of many different sizes for light and dark mode. Moreover if there is some kind of style change (like Windows 10 to 11)
    // we have to create different icons for different version of the OSes.
    float intrinsic_size = static_cast<float>(style()->pixelMetric(QStyle::PM_IndicatorWidth, nullptr, this));
    float font_size      = static_cast<float>(fontMetrics().ascent());
    float scale_factor   = 1.0;

    if (font_size != 0.0 && intrinsic_size != 0.0)
    {
        if (font_size < intrinsic_size)
        {
            // Determine how much the intrinsic size needs to be scaled down so that it's the same as the font size,
            // but make sure the divisor is a whole number so that the icon scales nicely.
            scale_factor = 1.0 / std::round(intrinsic_size / font_size);
        }
        else
        {
            // Determine how much the intrinsic size needs to be scaled up so that it's the same as the font size
            // and then round that to a whole number so that the icon scales nicely.
            scale_factor = std::round(font_size / intrinsic_size);
        }
    }

    // It appears the only way (or at least easiest way) to set the indicator size is through the stylesheet.
    int scaled_size = static_cast<int>(scale_factor * intrinsic_size);
    setStyleSheet(kIndicatorSizeStylesheet_.arg(scaled_size));
}

void ScaledCheckBox::OnScaleFactorChanged()
{
    // Invalidating the font metrics will ensure the metrics match the font size.
    // This will also trigger a fontChanged event, which is overridden in this class
    // to update the size of the indicator.
    QtCommon::QtUtils::InvalidateFontMetrics(this);

    // Update the indicator size to match the fonts.
    UpdateIndicatorSize();

    updateGeometry();
    update();
}

void ScaledCheckBox::changeEvent(QEvent* event)
{
    QCheckBox::changeEvent(event);

    if (!invalidating_font_metrics_)
    {
        if (event->type() == QEvent::FontChange)
        {
            invalidating_font_metrics_ = true;
            QtCommon::QtUtils::InvalidateFontMetrics(this);
            invalidating_font_metrics_ = false;

            UpdateIndicatorSize();

            updateGeometry();
        }
    }
}
