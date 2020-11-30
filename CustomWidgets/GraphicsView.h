//=============================================================================
/// Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for a graphics view that implements custom mouse work.
//=============================================================================

#ifndef _GRAPHICS_VIEW_H_
#define _GRAPHICS_VIEW_H_

#include <QGraphicsView>

/// Wavefront graphics view that is aware of resize and mouse events
class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    GraphicsView(QWidget* pParent);
    virtual ~GraphicsView() {}

    virtual void mouseMoveEvent(QMouseEvent* pEvent);
};

#endif // _GRAPHICS_VIEW_H_
