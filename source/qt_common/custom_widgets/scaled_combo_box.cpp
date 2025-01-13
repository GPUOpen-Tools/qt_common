//=============================================================================
/// Copyright (c) 2020-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QComboBox objects.
//=============================================================================
#include "scaled_combo_box.h"

#include <QFont>
#include <QLineEdit>

#include "qt_util.h"
#include "scaling_manager.h"

ScaledComboBox::ScaledComboBox(QWidget* parent)
    : QComboBox(parent)
{
    setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);

    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledComboBox::OnScaleFactorChanged);
}

ScaledComboBox::~ScaledComboBox()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledComboBox::OnScaleFactorChanged);
}

void ScaledComboBox::OnScaleFactorChanged()
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

    // If the ComboBox is editable and has a line edit widget, update it's font.
    QLineEdit* line_edit = lineEdit();
    if (line_edit != nullptr)
    {
        QtCommon::QtUtils::InvalidateFontMetrics(line_edit);
    }
}
