//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Base abstract scaling wrapper class. Scaling wrappers must define
/// two functions related to scaling:
///
/// SetCurrentAsBase() - Store values to represent the current state of the object.
/// ApplyScaling() - Use stored values as base (i.e. size in pixels at 100% scaling)
/// to perform scaling operations.
//=============================================================================

#ifndef _SCALED_OBJECT_WRAPPER_H_
#define _SCALED_OBJECT_WRAPPER_H_

class QObject;

/// Abstract QObject scaling wrapper
class ScaledObjectWrapper
{
public:
    explicit ScaledObjectWrapper(QObject* pObject) : m_pObject(pObject) {}
    virtual ~ScaledObjectWrapper() {}
    QObject* GetWrappedObject() { return m_pObject; }
    virtual void ApplyScaling() = 0;
    virtual void SetCurrentAsBase() = 0;

protected:
    QObject* m_pObject;             ///< Wrapped object pointer

};

#endif // _SCALED_OBJECT_WRAPPER_H_
