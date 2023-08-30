//=============================================================================
/// Copyright (c) 2016-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Headerfor a class that contains colored legends
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_COLORED_LEGEND_SCENE_H_
#define QTCOMMON_CUSTOM_WIDGETS_COLORED_LEGEND_SCENE_H_

#include <QGraphicsScene>

#include "qt_common/custom_widgets/graphics_scene.h"

class QGraphicsView;

/// An item in the colored legend.
struct ColorLegendItem
{
    /// Constructor
    ColorLegendItem()
        : rect_item_(nullptr)
        , text_item_()
    {
    }

    /// The colored rect associated with this item.
    QGraphicsRectItem* rect_item_;

    /// The text name of this item.
    QGraphicsTextItem* text_item_;
};

enum LegendMode
{
    kColor,
    kText
};

/// Support for colored legends.
class ColoredLegendScene : public QGraphicsScene
{
    Q_OBJECT

public:
    /// Explicit constructor
    /// \param parent The view displaying this widget.
    explicit ColoredLegendScene(QWidget* parent = nullptr);

    /// Virtual Destructor
    virtual ~ColoredLegendScene();

    /// Add a new box with a description beside it.
    /// \param color The color of the box
    /// \param description The legend description
    void AddColorLegendItem(const QColor& color, const QString& description);

    /// Add a string-only legend.
    /// \param description The legend description
    void AddTextLegendItem(const QString& description);

    /// Clear the scene.
    void Clear();

    /// Update item positions.
    void Update();

protected:
    LegendMode legend_mode_;  ///< Rendering either blocks with text, or just text

    QVector<ColorLegendItem>    color_legends_;  ///< Block to text legend pairings
    QVector<QGraphicsTextItem*> text_legends_;   ///< Plain text legends

    const int kHorizontalSpacingAfterText = 20;  ///< Horizontal spacing after the text, before the next item
    const int kVerticalSpacingAroundText  = 2;   ///< Vertical spacing above and below the text
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_COLORED_LEGEND_SCENE_H_
