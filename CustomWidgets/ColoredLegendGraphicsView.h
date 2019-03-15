//=============================================================================
/// Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for a graphics view that is aware of resize events.
//=============================================================================

#ifndef _COLORED_LEGEND_GRAPHICS_VIEW_H_
#define _COLORED_LEGEND_GRAPHICS_VIEW_H_

#include <QGraphicsView>

class ColoredLegendScene;

/// Colored legend graphics view that is aware of resize events
class ColoredLegendGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    ColoredLegendGraphicsView(QWidget* pParent);
    virtual ~ColoredLegendGraphicsView();

    void SetColoredLegendScene(ColoredLegendScene* pScene) { m_pColoredLegendScene = pScene; }

    virtual void resizeEvent(QResizeEvent* pEvent);

private:
    ColoredLegendScene* m_pColoredLegendScene;   ///< Reference to wavefront scene
};

#endif // _COLORED_LEGEND_GRAPHICS_VIEW_H_
