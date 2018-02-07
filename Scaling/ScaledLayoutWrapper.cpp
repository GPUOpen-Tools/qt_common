//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QLayout objects. Layouts perform a special
/// scaling operation to scale spacer items, as these are not QObjects and
/// cannot be scaled in the same way.
//=============================================================================

#include <ScaledLayoutWrapper.h>
#include <ScalingManager.h>

#include <QLayout>

//-----------------------------------------------------------------------------
/// Constructor.
/// \param pObject The object this class is wrapping.
//-----------------------------------------------------------------------------
ScaledLayoutWrapper::ScaledLayoutWrapper(QObject* pObject) :
    ScaledObjectWrapper(pObject),
    m_pLayout(static_cast<QLayout*>(pObject))
{
}

//-----------------------------------------------------------------------------
/// Destructor.
//-----------------------------------------------------------------------------
ScaledLayoutWrapper::~ScaledLayoutWrapper()
{
}

//-----------------------------------------------------------------------------
/// Apply scaling for this layout using the stored base scaling values.
//-----------------------------------------------------------------------------
void ScaledLayoutWrapper::ApplyScaling()
{
    ScalingManager& sm = ScalingManager::Get();

    // Set scaled values
    m_pLayout->setContentsMargins(sm.Scaled(m_marginLeft), sm.Scaled(m_marginTop), sm.Scaled(m_marginRight), sm.Scaled(m_marginBottom));
    m_pLayout->setSpacing(sm.Scaled(m_spacing));

    // Scale all spacer items
    for (SpacerItemWrapper& wrapper : m_spacerArray)
    {
        // Scale size
        QSize scaledSize = sm.Scaled(wrapper.size);

        // Get current size policy of the spacer
        QSizePolicy::Policy hPolicy = wrapper.pItem->sizePolicy().horizontalPolicy();
        QSizePolicy::Policy vPolicy = wrapper.pItem->sizePolicy().verticalPolicy();

        // Set spacer parameters with scaled size and unchanged size policies
        wrapper.pItem->changeSize(scaledSize.width(), scaledSize.height(), hPolicy, vPolicy);
    }
}

//-----------------------------------------------------------------------------
/// Store the base scaling values for this layout. This stores select data from the
/// layout in it's current state to be used when applying scaling. The layout's
/// base values determine the layout's appearance at 100% scaling.
//-----------------------------------------------------------------------------
void ScaledLayoutWrapper::SetCurrentAsBase()
{
    // Get layout values
    m_pLayout->getContentsMargins(&m_marginLeft, &m_marginTop, &m_marginRight, &m_marginBottom);
    m_spacing = m_pLayout->spacing();

    // Make list of spacer wrappers
    m_spacerArray.clear();
    for (int i = 0; i < m_pLayout->count(); i++)
    {
        QSpacerItem* pSpacer = m_pLayout->itemAt(i)->spacerItem();

        // Check if the item is a spacer (non-spacer items will return nullptr)
        if (pSpacer != nullptr)
        {
            // Create a scaling wrapper object for the spacer
            SpacerItemWrapper wrapper;
            wrapper.pItem = pSpacer;
            wrapper.size = pSpacer->sizeHint();

            // Add the spacer to the list
            m_spacerArray.push_back(wrapper);
        }
    }
}
