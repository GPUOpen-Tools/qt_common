//=============================================================================
/// Copyright (c) 2022-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QSpinBox objects.
//=============================================================================
#include "scaled_double_spin_box.h"

#include "qt_util.h"
#include "scaling_manager.h"

ScaledDoubleSpinBox::ScaledDoubleSpinBox(QWidget* parent)
    : QDoubleSpinBox(parent)
{
    setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);

    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledDoubleSpinBox::OnScaleFactorChanged);
}

ScaledDoubleSpinBox::~ScaledDoubleSpinBox()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledDoubleSpinBox::OnScaleFactorChanged);
}

void ScaledDoubleSpinBox::OnScaleFactorChanged()
{
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

    // The default QSpinBox has a bug that causes it to fill with black
    // if it is not explicitly told to update, this call to update() helps
    // avoid that issue (sometimes).
    update();
}
