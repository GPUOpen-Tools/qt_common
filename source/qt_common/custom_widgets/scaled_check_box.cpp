//=============================================================================
/// Copyright (c) 2019-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for ScaledCheckBox objects.
//=============================================================================
#include "scaled_check_box.h"

#include <cmath>

#include <QEvent>
#include <QStyle>

#include "qt_util.h"
#include "scaling_manager.h"

ScaledCheckBox::ScaledCheckBox(QWidget* parent)
    : ScaledCheckBox({}, parent)
{
}

ScaledCheckBox::ScaledCheckBox(const QString& text, QWidget* parent)
    : QCheckBox(text, parent)
    , invalidating_font_metrics_(false)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    // Use a queued connection so that the scaling logic runs on the next main loop iteration
    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledCheckBox::OnScaleFactorChanged, Qt::QueuedConnection);
}

ScaledCheckBox::~ScaledCheckBox()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledCheckBox::OnScaleFactorChanged);
}

void ScaledCheckBox::OnScaleFactorChanged()
{
    // Invalidating the font metrics will ensure the metrics match the font size.
    // This will also trigger a fontChanged event, which is overridden in this class
    // to update the size of the indicator.
    QtCommon::QtUtils::InvalidateFontMetrics(this);

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

            updateGeometry();
        }
    }
}
