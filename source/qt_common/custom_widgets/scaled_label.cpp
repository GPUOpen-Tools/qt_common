//=============================================================================
/// Copyright (c) 2019-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QLabel objects.
//=============================================================================
#include "scaled_label.h"

#include <QEvent>

#include "common_definitions.h"
#include "qt_util.h"
#include "scaling_manager.h"

ScaledLabel::ScaledLabel(QWidget* parent)
    : QLabel(parent)
    , invalidating_font_metrics_(false)
    , first_show_(true)
{
    setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledLabel::OnScaleFactorChanged);
}

ScaledLabel::~ScaledLabel()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledLabel::OnScaleFactorChanged);
}

void ScaledLabel::OnScaleFactorChanged()
{
    invalidating_font_metrics_ = true;
    QtCommon::QtUtils::InvalidateFontMetrics(this);
    invalidating_font_metrics_ = false;

    updateGeometry();
}

void ScaledLabel::changeEvent(QEvent* event)
{
    QLabel::changeEvent(event);

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

void ScaledLabel::showEvent(QShowEvent* show_event)
{
    QLabel::showEvent(show_event);

    if (first_show_ == true)
    {
        invalidating_font_metrics_ = true;
        QtCommon::QtUtils::InvalidateFontMetrics(this);
        invalidating_font_metrics_ = false;

        updateGeometry();

        first_show_ = false;
    }
}
