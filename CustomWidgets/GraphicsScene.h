//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header for a graphics scene that accepts drag and drop.
//=============================================================================

#ifndef _GRAPHICS_SCENE_H_
#define _GRAPHICS_SCENE_H_

#include <QGraphicsScene>
#include <QDropEvent>

/// Graphics scene that accepts drag and drop
class GraphicsScene : public QGraphicsScene
{
public:
    explicit GraphicsScene();
    explicit GraphicsScene(QRect rect, QObject* pView);

    virtual ~GraphicsScene() {}

    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent* pEvent);
    virtual void dropEvent(QGraphicsSceneDragDropEvent* pEvent);
};

#endif // _GRAPHICS_SCENE_H_
