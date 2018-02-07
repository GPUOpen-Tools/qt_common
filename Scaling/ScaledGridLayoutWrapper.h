//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QGridLayout object.
//=============================================================================

#ifndef _SCALED_GRID_LAYOUT_WRAPPER_H_
#define _SCALED_GRID_LAYOUT_WRAPPER_H_

#include <ScaledLayoutWrapper.h>

class QGridLayout;

/// QGridLayout scaling wrapper
class ScaledGridLayoutWrapper : public ScaledLayoutWrapper
{
public:
    explicit ScaledGridLayoutWrapper(QObject* pObject);
    virtual ~ScaledGridLayoutWrapper();
    virtual void ApplyScaling();
    virtual void SetCurrentAsBase();

private:
    int m_hSpacing;                 ///<< Base item horizontal spacing
    int m_vSpacing;                 ///<< Base item vertical spacing

    QGridLayout* m_pLayout;         ///< Wrapped object derived pointer
};

#endif // _SCALED_GRID_LAYOUT_WRAPPER_H_
