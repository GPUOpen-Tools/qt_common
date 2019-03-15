//=============================================================================
/// Copyright (c) 2016-2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  ColorGenerator header file
//=============================================================================

#ifndef _COLOR_GENERATOR_H_
#define _COLOR_GENERATOR_H_

#include <QColor>
#include <vector>
#include <random>

/// Class to generate a list of random colors
class ColorGenerator
{
public:
    ColorGenerator();
    ~ColorGenerator();

    QColor GetColor(size_t index);

private:
    int GetComponent(const int randomRange, const int offset);

    std::uniform_int_distribution<int>* m_pDistribution;     ///< The random distribution object
    std::vector<QColor>                 m_colorList;        ///< The generated list of random colors
};

#endif // _COLOR_GENERATOR_H_
