//=============================================================================
/// Copyright (c) 2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QGroupBox objects.
//=============================================================================
#include "scaled_group_box.h"

#include "utils/qt_util.h"
#include "utils/scaling_manager.h"

ScaledGroupBox::ScaledGroupBox(QWidget* parent)
    : QGroupBox(parent)
{
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledGroupBox::OnScaleFactorChanged);
}

ScaledGroupBox::ScaledGroupBox(const QString& text, QWidget* parent)
    : QGroupBox(text, parent)
{
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledGroupBox::OnScaleFactorChanged);
}

ScaledGroupBox::~ScaledGroupBox()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledGroupBox::OnScaleFactorChanged);
}

void ScaledGroupBox::OnScaleFactorChanged()
{
    QtCommon::QtUtils::InvalidateFontMetrics(this);

    if (parentWidget() != nullptr && parentWidget()->layout() != nullptr)
    {
        // If there is a parent widget and the parent has a layout,
        // then call updateGeometry, which works by recalculating the parent's layout
        // which should allow this widget to grow (or shrink) as needed.
        updateGeometry();
    }
    else
    {
        // If there is no parent or the parent doesn't have a layout,
        // then use adjustSize() so that this widget will grow (or shrink)
        // according to its sizeHint().
        adjustSize();
    }
}
