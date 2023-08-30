//=============================================================================
/// Copyright (c) 2016-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for a graphics view that is aware of the size
/// requirements of a ColoredLegendScene.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_COLORED_LEGEND_GRAPHICS_VIEW_H_
#define QTCOMMON_CUSTOM_WIDGETS_COLORED_LEGEND_GRAPHICS_VIEW_H_

#include <QGraphicsView>

/// Colored legend graphics view that is aware of resize events
class ColoredLegendGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    /// Constructor.
    /// \param parent The view's parent.
    ColoredLegendGraphicsView(QWidget* parent);

    /// Destructor.
    virtual ~ColoredLegendGraphicsView();

    /// Resizes the view to fit all the bounding items in the scene.
    void Update();

    /// Provide either a default size hint, or one based on the scene.
    /// \return The preferred size of this view.
    virtual QSize sizeHint() const;

public slots:
    /// Updates the size of the view based on the size of the items in the scene.
    /// This custom widget can do this because we know there will be a limited
    /// number of objects in the scene. This method is automatically called by Qt
    /// when a scene is added to a view, and when items are added or removed from
    /// the scene.
    /// \param rects The list of bounding rects in the scene.
    virtual void updateScene(const QList<QRectF>& rects);

private:
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_COLORED_LEGEND_GRAPHICS_VIEW_H_
