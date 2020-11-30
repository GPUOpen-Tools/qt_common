//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Implementation of a ColorPalette, which acts as a container for
/// QColors. Colors are accessed from the palette by their palette ids.
//=============================================================================

#include "ColorPalette.h"

#include <QTextStream>

const static QColor s_DEFAULT_COLOR(Qt::gray);

//-----------------------------------------------------------------------------
/// Constructor. Create a ColorPalette with the given number of colors.
/// \param count The number of different colors in this palette.
//-----------------------------------------------------------------------------
ColorPalette::ColorPalette(int count)
{
    m_colorArray.resize(count);
}

//-----------------------------------------------------------------------------
/// Constructor. Create a ColorPalette from a given palette string.
/// \param paletteString The palette string to create this palette from.
//-----------------------------------------------------------------------------
ColorPalette::ColorPalette(const QString& paletteString)
{
    setFromString(paletteString);
}

//-----------------------------------------------------------------------------
/// Destructor.
//-----------------------------------------------------------------------------
ColorPalette::~ColorPalette()
{
}

//-----------------------------------------------------------------------------
/// Get a color from the palette given a palette id.
/// \param paletteId The palette id of the color to be retrieved.
/// \return The corresponding color.
//-----------------------------------------------------------------------------
QColor ColorPalette::getColor(int paletteId) const
{
    // Use default color if palette id is outside the range of this palette
    if (paletteId >= m_colorArray.size())
    {
        return s_DEFAULT_COLOR;
    }

    return m_colorArray[paletteId];
}

//-----------------------------------------------------------------------------
/// Get the palette id of a given color. If no such color exists in this
/// palette then -1 is returned.
/// \param color The color to find the palette id of.
/// \return The palette id of the given color, or -1 if the color doesn't
/// exist in this palette.
//-----------------------------------------------------------------------------
int ColorPalette::getId(const QColor& color) const
{
    // Iterate through colors in palette array and compare them
    for (int i = 0; i < m_colorArray.size(); i++)
    {
        if (m_colorArray[i] == color)
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
/// Get the palette string representation of this palette. A palette string
/// consists of a series of comma separated hex color values.
/// \return The palette string representation of this palette.
//-----------------------------------------------------------------------------
QString ColorPalette::getString() const
{
    QString paletteString;

    // Build text stream of color values in hex form
    for (const QColor& color : m_colorArray)
    {
        QTextStream(&paletteString) << "#" << QString::number(color.rgb(), 16) << ",";
    }

    // Delete the very last character to eliminate the final comma
    paletteString.resize(paletteString.size() - 1);

    return paletteString;
}

//-----------------------------------------------------------------------------
/// Get the number of colors in this palette.
/// \return The palette color count.
//-----------------------------------------------------------------------------
int ColorPalette::getColorCount() const
{
    return m_colorArray.size();
}

//-----------------------------------------------------------------------------
/// Set the color at a given palette id.
/// \param paletteId The id of the palette item to edit the color of.
/// \param color The new color to assign to the palette item.
//-----------------------------------------------------------------------------
void ColorPalette::setColor(int paletteId, const QColor& color)
{
    m_colorArray[paletteId] = color;
}

//-----------------------------------------------------------------------------
/// Set the colors in this palette from a palette string. A palette string
/// consists of a series of comma separated hex color values.
/// \param string A palette string to set this palette from.
//-----------------------------------------------------------------------------
void ColorPalette::setFromString(const QString& string)
{
    QStringList c = string.split(",");
    int count = c.size();

    // Populate color array with colors from the string in sequence
    m_colorArray.resize(count);
    for (int i = 0; i < count; i++)
    {
        m_colorArray[i] = QColor(c.at(i));
    }
}
