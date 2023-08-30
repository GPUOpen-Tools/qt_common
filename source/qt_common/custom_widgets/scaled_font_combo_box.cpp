//=============================================================================
/// Copyright (c) 2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QFontComboBox objects.
//=============================================================================
#include <QFont>

#include "scaled_font_combo_box.h"

#include "scaling_manager.h"
#include "qt_util.h"

ScaledFontComboBox::ScaledFontComboBox(QWidget* parent)
    : QFontComboBox(parent)
{
    setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);

    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledFontComboBox::OnScaleFactorChanged);
}

ScaledFontComboBox::~ScaledFontComboBox()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledFontComboBox::OnScaleFactorChanged);
}

void ScaledFontComboBox::OnScaleFactorChanged()
{
    QtCommon::QtUtils::InvalidateFontMetrics(this);

    // Invalidate the font of each item in the item list.
    // This involves toggling the font in the fontRole,
    // so the same utility function cannot be used.
    QFont original_font = this->font();
    qreal pointsize     = original_font.pointSizeF();
    QFont invalidate_font = original_font;
    invalidate_font.setPointSizeF(pointsize + 1);

    int item_count = this->count();
    for (int i = 0; i < item_count; ++i)
    {
        this->setItemData(i, invalidate_font, Qt::FontRole);
        this->setItemData(i, original_font, Qt::FontRole);
    }

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
