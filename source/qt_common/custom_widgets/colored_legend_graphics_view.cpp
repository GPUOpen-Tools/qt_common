//=============================================================================
/// Copyright (c) 2016-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Implementation of a graphics view that is aware of the size
/// requirements of a ColoredLegendScene.
//=============================================================================

#include "colored_legend_graphics_view.h"

#include "scaling_manager.h"

ColoredLegendGraphicsView::ColoredLegendGraphicsView(QWidget* parent)
    : QGraphicsView(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
}

ColoredLegendGraphicsView::~ColoredLegendGraphicsView()
{
}

QSize ColoredLegendGraphicsView::sizeHint() const
{
    ensurePolished();

    QSize size;
    if (scene() == nullptr)
    {
        // If there is no scene, then set a default generic size of 100 pixels wide
        // and tall enough for a line of text.
        size.setWidth(100);
        size.setHeight(fontMetrics().height());
    }
    else
    {
        QRectF    rect   = scene()->itemsBoundingRect();
        const int width  = rect.width();
        const int height = rect.height();
        size.setWidth(width);
        size.setHeight(height);
    }

    return size;
}

void ColoredLegendGraphicsView::updateScene(const QList<QRectF>& rects)
{
    Update();

    QGraphicsView::updateScene(rects);
}

void ColoredLegendGraphicsView::Update()
{
    QRectF boundingRect = scene()->itemsBoundingRect();
    this->setFixedSize(boundingRect.width(), boundingRect.height());
}
