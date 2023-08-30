//=============================================================================
/// Copyright (c) 2017-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation for a graphics scene that accepts drag and drop.
//=============================================================================

#include "graphics_scene.h"

#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>

GraphicsScene::GraphicsScene()
{
}

GraphicsScene::GraphicsScene(QRect rect, QObject* view)
    : QGraphicsScene(rect, view)
{
}

void GraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
    if (event != nullptr)
    {
        QGraphicsScene::dragMoveEvent(event);

        if (event->mimeData()->hasUrls())
        {
            event->setDropAction(Qt::LinkAction);
            event->accept();
        }
    }
}

void GraphicsScene::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    Q_UNUSED(event);
}
