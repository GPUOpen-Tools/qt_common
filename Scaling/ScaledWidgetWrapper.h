//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QWidget object. The functionality for the
/// various things that need to be scaled (sizes, fonts, stylesheets, etc.) is
/// broken into a few functions to make it easier to use/not use those scaling
/// operations in derived wrapper classes.
//=============================================================================

#ifndef _SCALED_WIDGET_WRAPPER_H_
#define _SCALED_WIDGET_WRAPPER_H_

#include <QString>
#include <QSize>
#include <QRect>
#include <QFont>
#include <ScaledObjectWrapper.h>

class QWidget;

/// QWidget scaling wrapper
class ScaledWidgetWrapper : public ScaledObjectWrapper
{
public:
    ScaledWidgetWrapper(QObject* pObject);
    virtual ~ScaledWidgetWrapper();
    virtual void ApplyScaling();
    virtual void SetCurrentAsBase();

protected:
    void ScaleMaxAndMinSize();
    void ScaleGeometry();
    void ScaleFont();
    void ScaleStylesheet();

private:
    QSize m_maxSize;                ///< Base maximum size
    QSize m_minSize;                ///< Base minimum size
    QRect m_geometry;               ///< Base geometry
    QFont m_font;                   ///< Base font (pixel size is only important value)
    QString m_stylesheet;           ///< Base stylesheet string

    QWidget* m_pWidget;             ///< Wrapped object derived pointer

    bool m_doScaleStylesheet;       ///< Flag for whether stylesheet scaling is required
    bool m_doFontPixelScaling;      ///< Flag for whether font pixel scaling is required

};

#endif // _SCALED_WIDGET_WRAPPER_H_

