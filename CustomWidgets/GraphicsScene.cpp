//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation for a graphics scene that accepts drag and drop.
//=============================================================================

#include "GraphicsScene.h"

#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>

//-----------------------------------------------------------------------------
/// Constructor
//-----------------------------------------------------------------------------
GraphicsScene::GraphicsScene()
{
}

//-----------------------------------------------------------------------------
/// Constructor
/// \param rect scene rect
/// \param pView parent view
//-----------------------------------------------------------------------------
GraphicsScene::GraphicsScene(QRect rect, QObject* pView) :
    QGraphicsScene(rect, pView)
{
}

//-----------------------------------------------------------------------------
/// Handle what happens when dragging over the graphics view
/// \param pEvent the drag drop event
//-----------------------------------------------------------------------------
void GraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent* pEvent)
{
    if (pEvent != nullptr)
    {
        QGraphicsScene::dragMoveEvent(pEvent);

        if (pEvent->mimeData()->hasUrls())
        {
            pEvent->setDropAction(Qt::LinkAction);
            pEvent->accept();
        }
    }
}

//-----------------------------------------------------------------------------
/// Handle what happens when dropping something on the graphics view
/// \param pEvent the drag drop event
//-----------------------------------------------------------------------------
void GraphicsScene::dropEvent(QGraphicsSceneDragDropEvent* pEvent)
{
    Q_UNUSED(pEvent);
}
