//=============================================================================
/// Copyright (c) 2016-2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Headerfor a class that contains colored legends
//=============================================================================

#ifndef _COLORED_LEGEND_SCENE_H_
#define _COLORED_LEGEND_SCENE_H_

#include <QGraphicsScene>

#include <../Util/CommonDefinitions.h>
#include <../CustomWidgets/GraphicsScene.h>

class QGraphicsView;

struct TextLegendItem
{
    QGraphicsTextItem* pTextItem;
    int                textWidth;
};

struct ColorLegendItem
{
    QGraphicsRectItem* pRectItem;
    TextLegendItem     textInfo;
};

enum LegendMode
{
    LEGEND_MODE_COLOR,
    LEGEND_MODE_TEXT
};

/// Support for the RGP-style completion bar.
class ColoredLegendScene
{
public:
    // Implementations for Qt
    explicit ColoredLegendScene();
    virtual ~ColoredLegendScene();

    void ResizeEvent(const QSize &newSize);

    GraphicsScene* Scene() { return m_pScene; }

    void AddColorLegendItem(const QColor& color, const QString& desc);
    void AddTextLegendItem(const QString& str);

    void Clear();
    void Update();

protected:
    GraphicsScene*      m_pScene;               ///< The Qt scene holding our legends
    int                 m_width;                ///< Scene width
    int                 m_height;               ///< Scene height
    LegendMode          m_legendMode;           ///< Rendering either blocks with text, or just text
    QFont               m_font;                 ///< The font used to render the text

    QVector<ColorLegendItem>  m_colorLegends;   ///< Block to text legend pairings
    QVector<TextLegendItem>   m_textLegends;    ///< Plain text legends
};

#endif // _COLORED_LEGEND_SCENE_H_
