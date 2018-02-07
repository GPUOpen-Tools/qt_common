//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QMainWindow object.
//=============================================================================

#ifndef _SCALED_MAIN_WINDOW_WRAPPER_H_
#define _SCALED_MAIN_WINDOW_WRAPPER_H_

#include <ScaledWidgetWrapper.h>

class QMainWindow;

/// QMainWindow scaling wrapper
class ScaledMainWindowWrapper : public ScaledWidgetWrapper
{
public:
    ScaledMainWindowWrapper(QObject* pObject);
    virtual ~ScaledMainWindowWrapper();
    virtual void ApplyScaling();

private:
    QMainWindow* m_pMainWindow;

};

#endif // _SCALED_MAIN_WINDOW_WRAPPER_H_

