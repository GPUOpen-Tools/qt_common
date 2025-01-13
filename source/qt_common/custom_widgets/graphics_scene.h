//=============================================================================
/// Copyright (c) 2017-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header for a graphics scene that accepts drag and drop.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_GRAPHICS_SCENE_H_
#define QTCOMMON_CUSTOM_WIDGETS_GRAPHICS_SCENE_H_

#include <QGraphicsScene>
#include <QDropEvent>

/// Graphics scene that accepts drag and drop.
class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    /// Constructor
    explicit GraphicsScene();

    /// Constructor
    /// \param rect scene rect.
    /// \param view parent view.
    explicit GraphicsScene(QRect rect, QObject* view);

    virtual ~GraphicsScene()
    {
    }

    /// Handle what happens when dragging over the graphics view.
    /// \param event The drag drop event.
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent* event);

    /// Handle what happens when dropping something on the graphics view.
    /// \param event The drag drop event.
    virtual void dropEvent(QGraphicsSceneDragDropEvent* event);
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_GRAPHICS_SCENE_H_
