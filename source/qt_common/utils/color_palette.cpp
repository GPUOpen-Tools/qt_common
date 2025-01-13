//=============================================================================
/// Copyright (c) 2017-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Implementation of a ColorPalette, which acts as a container for
/// QColors. Colors are accessed from the palette by their palette ids.
//=============================================================================

#include "color_palette.h"

#include <QTextStream>

const static QColor kDefaultColor(Qt::gray);

ColorPalette::ColorPalette(int count)
{
    color_array_.resize(count);
}

ColorPalette::ColorPalette(const QString& palette_string)
{
    SetFromString(palette_string);
}

ColorPalette::~ColorPalette()
{
}

QColor ColorPalette::GetColor(int palette_id) const
{
    // Use default color if palette id is outside the range of this palette
    if (palette_id >= color_array_.size())
    {
        return kDefaultColor;
    }

    return color_array_[palette_id];
}

int ColorPalette::GetId(const QColor& color) const
{
    // Iterate through colors in palette array and compare them
    for (int i = 0; i < color_array_.size(); i++)
    {
        if (color_array_[i] == color)
        {
            return i;
        }
    }

    return -1;
}

QString ColorPalette::GetString() const
{
    QString palette_string;

    // Build text stream of color values in hex form
    for (const QColor& color : color_array_)
    {
        QTextStream(&palette_string) << "#" << QString::number(color.rgb(), 16) << ",";
    }

    // Delete the very last character to eliminate the final comma
    palette_string.resize(palette_string.size() - 1);

    return palette_string;
}

int ColorPalette::GetColorCount() const
{
    return color_array_.size();
}

void ColorPalette::SetColor(int palette_id, const QColor& color)
{
    color_array_[palette_id] = color;
}

void ColorPalette::SetFromString(const QString& string)
{
    QStringList c     = string.split(",");
    int         count = c.size();

    // Populate color array with colors from the string in sequence
    color_array_.resize(count);
    for (int i = 0; i < count; i++)
    {
        color_array_[i] = QColor(c.at(i));
    }
}
