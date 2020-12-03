//=============================================================================
/// Copyright (c) 2016-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  ColorGenerator header file
//=============================================================================

#ifndef QTCOMMON_UTILS_COLOR_GENERATOR_H_
#define QTCOMMON_UTILS_COLOR_GENERATOR_H_

#include <QColor>
#include <random>
#include <vector>

/// Class to generate a list of random colors
class ColorGenerator
{
public:
    /// Constructor
    ColorGenerator();

    /// Destructor
    ~ColorGenerator();

    /// Get the color at a specified index. If the index doesn't exist, the list
    /// is expanded until the color is available.
    /// \param index The required color index
    /// \return the color corresponding to the index provided
    QColor GetColor(size_t index);

private:
    /// Calculate the value of a color component.
    /// \param random_range Will generate an int between 0 and random_range
    /// \param offset The value to be added to the component
    int GetComponent(const int random_range, const int offset);

    std::uniform_int_distribution<int>* distribution_;  ///< The random distribution object
    std::vector<QColor>                 color_list_;    ///< The generated list of random colors
};

#endif  // QTCOMMON_UTILS_COLOR_GENERATOR_H_
