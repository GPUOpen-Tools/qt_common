//=============================================================================
/// Copyright (c) 2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling cycle button that cycles through a set of options each click.
//=============================================================================
#include "scaled_cycle_button.h"

#include <QStyle>

#include "utils/scaling_manager.h"

ScaledCycleButton::ScaledCycleButton(QWidget* parent)
    : QPushButton(parent)
    , init_(false)
    , current_item_(-1)
{
}

ScaledCycleButton::ScaledCycleButton(const QStringList& text_items, QWidget* parent)
    : QPushButton(parent)
    , init_(false)
    , current_item_(-1)
{
    Init(text_items);
}

ScaledCycleButton::~ScaledCycleButton()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &QPushButton::updateGeometry);
    disconnect(this, &QPushButton::clicked, this, &ScaledCycleButton::NextItem);
}

void ScaledCycleButton::Init(const QStringList& text_items)
{
    if (init_ == false)
    {
        connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &QPushButton::updateGeometry);
        connect(this, &QPushButton::clicked, this, &ScaledCycleButton::NextItem);
        init_ = true;
    }

    text_items_ = text_items;
    if (text_items_.isEmpty() == true)
    {
        current_item_ = -1;
    }
    else
    {
        current_item_ = 0;
    }

    SetCurrentItemIndex(current_item_);
}

void ScaledCycleButton::NextItem(bool enabled)
{
    Q_UNUSED(enabled);

    if (text_items_.isEmpty() == true)
    {
        return;
    }

    current_item_++;
    if (current_item_ >= text_items_.count())
    {
        current_item_ = 0;
    }

    SetCurrentItemIndex(current_item_);
    emit Clicked(current_item_);
}

int ScaledCycleButton::CurrentItemIndex() const
{
    return current_item_;
}

bool ScaledCycleButton::SetCurrentItemIndex(int index)
{
    if ((text_items_.isEmpty() == true) || (index >= text_items_.count()))
    {
        return false;
    }

    current_item_ = index;
    setText(text_items_[current_item_]);
    return true;
}

QSize ScaledCycleButton::sizeHint() const
{
    ensurePolished();

    // Create a scaled FontMetrics for measuring the resized fonts
    ScalingManager&     sm             = ScalingManager::Get();
    const QFontMetrics& scaled_metrics = sm.ScaledFontMetrics(font());

    // Find the longest string used by the cycle button.
    QSize largest_scaled_size;
    for (const auto& text_string : text_items_)
    {
        QSize scaled_size = scaled_metrics.size(0, text_string);
        if (scaled_size.width() > largest_scaled_size.width())
        {
            largest_scaled_size = scaled_size;
        }
    }
    QSize size_hint = largest_scaled_size;

    // Adjust the button size for margins.
    int button_margin = sm.Scaled(style()->pixelMetric(QStyle::PM_ButtonMargin));
    size_hint.setWidth(size_hint.width() + button_margin * 2);
    size_hint.setHeight(size_hint.height() + button_margin * 2);

    return size_hint;
}
