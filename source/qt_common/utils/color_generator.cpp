//=============================================================================
/// Copyright (c) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of the ColorGenerator. This is used to create a
/// dynamic array of colors. As new colors are needed, they are generated and
/// added to the array. This is used in the UI when showing events colored
/// by harware stage or by scene.
//=============================================================================

#include "color_generator.h"

#include <iostream>

static const int kMaxBrightness = 208;
static const int kMaxRandom     = 65535;
static const int kMinHueDiff    = 45;
static const int kMinSaturation = 96;

/// The random number generation algorithm
static std::mt19937 sRandomAlgorithm(20);

ColorGenerator::ColorGenerator()
{
    distribution_ = new std::uniform_int_distribution<int>(0, kMaxRandom);
}

ColorGenerator::~ColorGenerator()
{
    delete distribution_;
}

int ColorGenerator::GetComponent(const int random_range, const int offset)
{
    int component = (*distribution_)(sRandomAlgorithm);
    component *= random_range;
    component /= kMaxRandom;
    component += offset;

    return component;
}

QColor ColorGenerator::GetColor(size_t index)
{
    size_t size_needed = index + 1;
    size_t size        = color_list_.size();
    if (size_needed > size)
    {
        int last_hue = 0;
        if (size > 0)
        {
            last_hue = color_list_[size - 1].hue();
        }

        size_t count = size_needed - size;
        for (size_t loop = 0; loop < count; loop++)
        {
            // Calculate HSV values for color rather than RGB to avoid potential infinite loop
            int hue        = GetComponent(359 - (2 * kMinHueDiff), 0);
            int saturation = GetComponent(255 - kMinSaturation, kMinSaturation);
            int value      = GetComponent(255 - kMaxBrightness, kMaxBrightness);

            hue += last_hue;
            if (hue > 360)
            {
                hue -= 360;
            }

            QColor color;
            color.setHsv(hue, saturation, value);
            color_list_.push_back(color);
            last_hue = hue;
        }
    }

    return color_list_[index];
}

void ColorGenerator::ReseedColors(uint64_t new_seed)
{
    sRandomAlgorithm.seed(new_seed);
    color_list_.clear();
}
