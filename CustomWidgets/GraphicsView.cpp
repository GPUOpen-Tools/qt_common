//=============================================================================
/// Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Implementation of a graphics view that implements custom mouse work.
//=============================================================================

#include "GraphicsView.h"

#include <QDebug>

//-----------------------------------------------------------------------------
/// Constructor.
/// \param pParent The view's parent.
//-----------------------------------------------------------------------------
GraphicsView::GraphicsView(QWidget* pParent) :
    QGraphicsView(pParent)
{
    setMouseTracking(true);
}

//-----------------------------------------------------------------------------
/// Capture a mouse move event.
/// \param pEvent The move event.
//-----------------------------------------------------------------------------
void GraphicsView::mouseMoveEvent(QMouseEvent* pEvent)
{
    QGraphicsView::mouseMoveEvent(pEvent);

#if 0
        QPoint mouseCoords = mapFromGlobal(QCursor::pos());
        QPointF sceneCoords = mapToScene(mouseCoords);
        qDebug() << "mouse: " << mouseCoords;
        qDebug() << "sceneCoords: " << sceneCoords;
        qDebug() << "---------";
#endif
}
