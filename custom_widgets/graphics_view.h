//=============================================================================
/// Copyright (c) 2018-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for a graphics view that implements custom mouse work.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_GRAPHICS_VIEW_H_
#define QTCOMMON_CUSTOM_WIDGETS_GRAPHICS_VIEW_H_

#include <QGraphicsView>

/// Wavefront graphics view that is aware of resize and mouse events.
class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    /// Constructor.
    /// \param parent The view's parent.
    GraphicsView(QWidget* pParent);

    /// Destructor.
    virtual ~GraphicsView();

    /// Override the mouse move event.
    /// \param event The move event.
    virtual void mouseMoveEvent(QMouseEvent* pEvent) Q_DECL_OVERRIDE;
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_GRAPHICS_VIEW_H_
