//=============================================================================
/// Copyright (c) 2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QTabWidget objects.
//=============================================================================
#include "scaled_tab_widget.h"

#include <QTabBar>

#include "qt_util.h"
#include "scaling_manager.h"

ScaledTabWidget::ScaledTabWidget(QWidget* parent)
    : QTabWidget(parent)
{
    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);

    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledTabWidget::OnScaleFactorChanged);
}

ScaledTabWidget::~ScaledTabWidget()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledTabWidget::OnScaleFactorChanged);
}

void ScaledTabWidget::OnScaleFactorChanged()
{
    tabBar()->setFont(this->font());

    QtCommon::QtUtils::InvalidateFontMetrics(this);

    // Update the font of the tab bar as well
    QtCommon::QtUtils::InvalidateFontMetrics(tabBar());
    this->tabBar()->updateGeometry();

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
