//=============================================================================
/// Copyright (c) 2016-2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of the ColorGenerator. This is used to create a
/// dynamic array of colors. As new colors are needed, they are generated and
/// added to the array. This is used in the UI when showing events colored
/// by harware stage or by scene.
//=============================================================================

#include <iostream>

#include "ColorGenerator.h"

static const int s_MAX_BRIGHTNESS = 208;
static const int s_MAX_RANDOM = 65535;
static const int s_MIN_HUE_DIFF = 45;
static const int s_MIN_SATURATION = 96;

// The random number generation algorithm
static std::mt19937 s_randomAlgorithm(20);

//-----------------------------------------------------------------------------
/// Constructor
//-----------------------------------------------------------------------------
ColorGenerator::ColorGenerator()
{
    m_pDistribution = new std::uniform_int_distribution<int>(0, s_MAX_RANDOM);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ColorGenerator::~ColorGenerator()
{
    delete m_pDistribution;
}

//-----------------------------------------------------------------------------
/// Calculate the value of a color component.
/// \param randomRange Will generate an int between 0 and randomRange
/// \param offset The value to be added to the component
//-----------------------------------------------------------------------------
int ColorGenerator::GetComponent(const int randomRange, const int offset)
{
    int component = (*m_pDistribution)(s_randomAlgorithm);
    component *= randomRange;
    component /= s_MAX_RANDOM;
    component += offset;

    return component;
}

//-----------------------------------------------------------------------------
/// Get the color at a specified index. If the index doesn't exist, the list
/// is expanded until the color is available.
/// \param index The required color index
/// \return the color corresponding to the index provided
//-----------------------------------------------------------------------------
QColor ColorGenerator::GetColor(size_t index)
{
    size_t sizeNeeded = index + 1;
    size_t size = m_colorList.size();
    if (sizeNeeded > size)
    {
        int lastHue = 0;
        if (size > 0)
        {
            lastHue = m_colorList[size - 1].hue();
        }

        size_t count = sizeNeeded - size;
        for (size_t loop = 0; loop < count; loop++)
        {
            // Calculate HSV values for color rather than RGP to avoid potential infinite loop
            int hue = GetComponent(359 - (2 * s_MIN_HUE_DIFF), 0);
            int saturation = GetComponent(255 - s_MIN_SATURATION, s_MIN_SATURATION);
            int value = GetComponent(255 - s_MAX_BRIGHTNESS, s_MAX_BRIGHTNESS);

            hue += lastHue;
            if (hue > 360)
            {
                hue -= 360;
            }

            QColor color;
            color.setHsv(hue, saturation, value);
            m_colorList.push_back(color);
            lastHue = hue;
        }
    }

    return m_colorList[index];
}

