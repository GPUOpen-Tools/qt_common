//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QGridLayout object.
//=============================================================================

#include <ScaledGridLayoutWrapper.h>
#include <ScalingManager.h>

#include <QGridLayout>

//-----------------------------------------------------------------------------
/// Constructor.
/// \param pObject The object this class is wrapping.
//-----------------------------------------------------------------------------
ScaledGridLayoutWrapper::ScaledGridLayoutWrapper(QObject* pObject) :
    ScaledLayoutWrapper(pObject),
    m_pLayout(static_cast<QGridLayout*>(pObject))
{
}

//-----------------------------------------------------------------------------
/// Destructor.
//-----------------------------------------------------------------------------
ScaledGridLayoutWrapper::~ScaledGridLayoutWrapper()
{
}

//-----------------------------------------------------------------------------
/// Apply scaling for this grid layout using the stored base scaling values.
//-----------------------------------------------------------------------------
void ScaledGridLayoutWrapper::ApplyScaling()
{
    ScalingManager& sm = ScalingManager::Get();

    // Parent wrapper scaling
    ScaledLayoutWrapper::ApplyScaling();

    // Set scaled values
    m_pLayout->setHorizontalSpacing(sm.Scaled(m_hSpacing));
    m_pLayout->setVerticalSpacing(sm.Scaled(m_vSpacing));
}

//-----------------------------------------------------------------------------
/// Store the base scaling values for this grid layout. This stores select data
/// from the grid layout in it's current state to be used when applying scaling.
/// The grid layout's base values determine the grid layout's appearance at 100%
/// scaling.
//-----------------------------------------------------------------------------
void ScaledGridLayoutWrapper::SetCurrentAsBase()
{
    // Get parent layout values
    ScaledLayoutWrapper::SetCurrentAsBase();

    // Get layout values
    m_hSpacing = m_pLayout->horizontalSpacing();
    m_vSpacing = m_pLayout->verticalSpacing();
}
