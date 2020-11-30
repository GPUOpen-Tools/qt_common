//=============================================================================
/// Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Implementation of a graphics view that is aware of resize events.
//=============================================================================

#include "ColoredLegendGraphicsView.h"
#include "ColoredLegendScene.h"

//-----------------------------------------------------------------------------
/// Constructor.
/// \param pParent The view's parent.
//-----------------------------------------------------------------------------
ColoredLegendGraphicsView::ColoredLegendGraphicsView(QWidget* pParent) :
    QGraphicsView(pParent),
    m_pColoredLegendScene(nullptr)
{
}

//-----------------------------------------------------------------------------
/// Destructor.
/// \param pParent The view's parent.
//-----------------------------------------------------------------------------
ColoredLegendGraphicsView::~ColoredLegendGraphicsView()
{
}

//-----------------------------------------------------------------------------
/// Capture a resize event.
/// \param pEvent The resize event.
//-----------------------------------------------------------------------------
void ColoredLegendGraphicsView::resizeEvent(QResizeEvent* pEvent)
{
    QGraphicsView::resizeEvent(pEvent);

    if (m_pColoredLegendScene != nullptr)
    {
        m_pColoredLegendScene->ResizeEvent(size());
    }
}
