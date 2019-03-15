//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for a ColorPalette
//=============================================================================

#ifndef _COLOR_PALETTE_H_
#define _COLOR_PALETTE_H_

#include <QVector>
#include <QColor>
#include <QString>

/// ColorPalette implementation
class ColorPalette
{
public:
    ColorPalette(int count);
    ColorPalette(const QString& paletteString);
    virtual ~ColorPalette();

    QColor getColor(int paletteId) const;
    int getId(const QColor& color) const;
    QString getString() const;
    int getColorCount() const;
    void setColor(int paletteId, const QColor& color);
    void setFromString(const QString& string);

private:
    QVector<QColor> m_colorArray;       ///< Array of colors used in this palette
};

#endif // _COLOR_PALETTE_H_
