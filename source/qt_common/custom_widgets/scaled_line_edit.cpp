//=============================================================================
/// Copyright (c) 2020-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QLineEdit objects.
//=============================================================================
#include "scaled_line_edit.h"

#include "qt_util.h"
#include "scaling_manager.h"

ScaledLineEdit::ScaledLineEdit(QWidget* parent)
    : QLineEdit(parent)
{

    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledLineEdit::OnScaleFactorChanged);
}

ScaledLineEdit::ScaledLineEdit(const QString& text, QWidget* parent)
    : QLineEdit(text, parent)
{
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledLineEdit::OnScaleFactorChanged);
}

ScaledLineEdit::~ScaledLineEdit()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledLineEdit::OnScaleFactorChanged);
}

void ScaledLineEdit::OnScaleFactorChanged()
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

    // The default QLineEdit has a bug that causes it to fill with black
    // if it is not explicitly told to update, so do that here.
    update();
}
