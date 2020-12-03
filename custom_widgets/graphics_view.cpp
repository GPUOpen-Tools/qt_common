//=============================================================================
/// Copyright (c) 2018-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Implementation of a graphics view that implements custom mouse work.
//=============================================================================

#include "graphics_view.h"

#include <QDebug>

GraphicsView::GraphicsView(QWidget* parent)
    : QGraphicsView(parent)
{
    setMouseTracking(true);
}

GraphicsView::~GraphicsView()
{
}

void GraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    QGraphicsView::mouseMoveEvent(event);

#if 0
        QPoint mouse_coords = mapFromGlobal(QCursor::pos());
        QPointF scene_coords = mapToScene(mouse_coords);
        qDebug() << "mouse_coords: " << mouse_coords;
        qDebug() << "scene_coords: " << scene_coords;
        qDebug() << "---------";
#endif
}
