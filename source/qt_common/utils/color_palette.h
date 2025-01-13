//=============================================================================
/// Copyright (c) 2017-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for a ColorPalette
//=============================================================================

#ifndef QTCOMMON_UTILS_COLOR_PALETTE_H_
#define QTCOMMON_UTILS_COLOR_PALETTE_H_

#include <QVector>
#include <QColor>
#include <QString>

/// ColorPalette implementation
class ColorPalette
{
public:
    /// Constructor which creates a ColorPalette with the given number of colors.
    /// \param count The number of different colors in this palette.
    ColorPalette(int count);

    /// Constructor which creates a ColorPalette from a given palette string.
    /// \param palette_string The palette string to create this palette from.
    ColorPalette(const QString& palette_string);

    /// Destructor
    virtual ~ColorPalette();

    /// Get a color from the palette given a palette id.
    /// \param palette_id The palette id of the color to be retrieved.
    /// \return The corresponding color.
    QColor GetColor(int palette_id) const;

    /// Get the palette id of a given color. If no such color exists in this
    /// palette then -1 is returned.
    /// \param color The color to find the palette id of.
    /// \return The palette id of the given color, or -1 if the color doesn't
    /// exist in this palette.
    int GetId(const QColor& color) const;

    /// Get the palette string representation of this palette. A palette string
    /// consists of a series of comma separated hex color values.
    /// \return The palette string representation of this palette.
    QString GetString() const;

    /// Get the number of colors in this palette.
    /// \return The palette color count.
    int GetColorCount() const;

    /// Set the color at a given palette id.
    /// \param palette_id The id of the palette item to edit the color of.
    /// \param color The new color to assign to the palette item.
    void SetColor(int palette_id, const QColor& color);

    /// Set the colors in this palette from a palette string. A palette string
    /// consists of a series of comma separated hex color values.
    /// \param string A palette string to set this palette from.
    void SetFromString(const QString& string);

private:
    /// Array of colors used in this palette.
    QVector<QColor> color_array_;
};

#endif  // QTCOMMON_UTILS_COLOR_PALETTE_H_
