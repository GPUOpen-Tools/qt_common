//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Implementation of ScaledPaneWidget
//=============================================================================

#include "ScaledPaneWidget.h"
#include <../Scaling/ScalingManager.h>

const static int s_MAX_SIZE = 16777215;

//-----------------------------------------------------------------------------
/// Explicit constructor.
/// \param pParent The widget's parent.
//-----------------------------------------------------------------------------
ScaledPaneWidget::ScaledPaneWidget(QWidget* pParent) :
    QWidget(pParent)
{
    connect(&ScalingManager::Get(), SIGNAL(ScaleFactorChanged(double, double)), this, SLOT(adjustScaling(double, double)));

    // Set default base geometry values
    m_baseGeometry = QRect(0, 0, 0, 0);
    m_baseMinSize = QSize(0, 0);
    m_baseMaxSize = QSize(s_MAX_SIZE, s_MAX_SIZE);
    m_baseStyleSheet.clear();
}

//-----------------------------------------------------------------------------
/// Destructor.
//-----------------------------------------------------------------------------
ScaledPaneWidget::~ScaledPaneWidget()
{
}

//-----------------------------------------------------------------------------
/// Set geometry override - sets the geometry with a scale factor applied.
/// \param rect The desired base geometry.
//-----------------------------------------------------------------------------
void ScaledPaneWidget::setGeometry(const QRect& rect)
{
    m_baseGeometry = rect;

    // Set geometry with scaling
    ScalingManager& sm = ScalingManager::Get();
    QWidget::setGeometry(sm.Scaled(rect));
}

//-----------------------------------------------------------------------------
/// Set minimum size override - sets the minimum size with a scale factor applied.
/// \param rect The desired base minimum size.
//-----------------------------------------------------------------------------
void ScaledPaneWidget::setMinimumSize(const QSize& size)
{
    m_baseMinSize = size;

    // Set minimum size with scaling
    ScalingManager& sm = ScalingManager::Get();
    QWidget::setMinimumSize(sm.Scaled(size).expandedTo(QSize(0, 0)));
}

//-----------------------------------------------------------------------------
/// Set maximum size override - sets the maximum size with a scale factor applied.
/// \param rect The desired base maximum size.
//-----------------------------------------------------------------------------
void ScaledPaneWidget::setMaximumSize(const QSize& size)
{
    m_baseMaxSize = size;

    // Set maximum size with scaling
    ScalingManager& sm = ScalingManager::Get();
    QWidget::setMaximumSize(sm.Scaled(size).boundedTo(QSize(s_MAX_SIZE, s_MAX_SIZE)));
}

//-----------------------------------------------------------------------------
/// Set stylesheet override - sets the stylesheet with a scale factor applied
/// to every length given in pixels (every place in the stylesheet with ##px
/// will have the numerical component scaled).
/// \param stlyeSheet The base stylesheet to set.
//-----------------------------------------------------------------------------
void ScaledPaneWidget::setStyleSheet(const QString& styleSheet)
{
    m_baseStyleSheet = styleSheet;

    ScalingManager& sm = ScalingManager::Get();
    QWidget::setStyleSheet(sm.ScaledStylesheet(styleSheet));
}

//-----------------------------------------------------------------------------
/// Scaling adjust slot - called when the system scale factor changes.
//-----------------------------------------------------------------------------
void ScaledPaneWidget::adjustScaling(double oldScaleFactor, double newScaleFactor)
{
    Q_UNUSED(oldScaleFactor);
    Q_UNUSED(newScaleFactor);

    // Reset all properties from base values
    setGeometry(m_baseGeometry);
    setMinimumSize(m_baseMinSize);
    setMaximumSize(m_baseMaxSize);

    // only set style sheet if there is one
    if (!m_baseStyleSheet.isEmpty())
    {
        setStyleSheet(m_baseStyleSheet);
    }

    // Widget update
    update();
}
