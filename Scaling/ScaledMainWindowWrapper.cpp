//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QMainWindow object.
//=============================================================================

#include <ScaledMainWindowWrapper.h>
#include <ScalingManager.h>

#include <QMainWindow>
#include <QSize>
#include <QPoint>
#include <QResizeEvent>
#include <QApplication>

//-----------------------------------------------------------------------------
/// Constructor.
/// \param pObject The object this class is wrapping.
//-----------------------------------------------------------------------------
ScaledMainWindowWrapper::ScaledMainWindowWrapper(QObject* pObject) :
    ScaledWidgetWrapper(pObject),
    m_pMainWindow(static_cast<QMainWindow*>(pObject))
{
}

//-----------------------------------------------------------------------------
/// Destructor.
//-----------------------------------------------------------------------------
ScaledMainWindowWrapper::~ScaledMainWindowWrapper()
{
}

//-----------------------------------------------------------------------------
/// Apply scaling for this main window using the stored base scaling values.
//-----------------------------------------------------------------------------
void ScaledMainWindowWrapper::ApplyScaling()
{
    ScalingManager& sm = ScalingManager::Get();

    // Get window size for rescaling (because of the way the max/min size affects the resize operation
    // we gather the rescaling size BEFORE scaling the max/min, but the actual rescale must
    // happen AFTER scaling the max/min)
    QSize windowSize = m_pMainWindow->size();

    // QWidget scaling operations
    ScaleMaxAndMinSize();
    ScaleFont();
    ScaleStylesheet();

    // Rescale the main window size
    m_pMainWindow->resize(sm.Rescaled(windowSize));

    // Post a resize event for the main window to force it to re-layout components
    QResizeEvent* pEvent = new QResizeEvent(m_pMainWindow->size(), m_pMainWindow->size());
    QApplication::postEvent(m_pMainWindow, pEvent);
}
