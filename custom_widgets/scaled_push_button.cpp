//=============================================================================
/// Copyright (c) 2019-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QPushButton objects.
//=============================================================================
#include "scaled_push_button.h"

#include "utils/qt_util.h"
#include "utils/scaling_manager.h"

ScaledPushButton::ScaledPushButton(QWidget* parent)
    : QPushButton(parent)
{
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledPushButton::OnScaleFactorChanged);
}

ScaledPushButton::ScaledPushButton(const QString& text, QWidget* parent)
    : QPushButton(text, parent)
{
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledPushButton::OnScaleFactorChanged);
}

ScaledPushButton::~ScaledPushButton()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledPushButton::OnScaleFactorChanged);
}

void ScaledPushButton::OnScaleFactorChanged()
{
    QtCommon::QtUtils::InvalidateFontMetrics(this);

    if (parentWidget() != nullptr && parentWidget()->layout() != nullptr)
    {
        // If there is a parent widget and the parent has a layout,
        // then call updateGeometry, which works by recalculating the parent's layout
        // which should allow this ScaledPushButton to grow (or shrink) as needed.
        updateGeometry();
    }
    else
    {
        // If there is no parent or the parent doesn't have a layout,
        // then use adjustSize() so that this ScaledPushButton will grow (or shrink)
        // according to its sizeHint().
        // This can be the case for buttons that are added to a TabBar as a tabButton.
        adjustSize();
    }
}
