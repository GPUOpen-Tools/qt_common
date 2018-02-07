//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QLayout objects. Layouts perform a special
/// scaling operation to scale spacer items, as these are not QObjects and
/// cannot be scaled in the same way.
//=============================================================================

#ifndef _SCALED_LAYOUT_WRAPPER_H_
#define _SCALED_LAYOUT_WRAPPER_H_

#include <QSize>
#include <QVector>
#include <ScaledObjectWrapper.h>

class QLayout;
class QSpacerItem;

/// QLayout scaling wrapper
class ScaledLayoutWrapper : public ScaledObjectWrapper
{
public:
    ScaledLayoutWrapper(QObject* pObject);
    virtual ~ScaledLayoutWrapper();
    virtual void ApplyScaling();
    virtual void SetCurrentAsBase();

private:
    int m_marginLeft;               ///<< Base left edge margin
    int m_marginRight;              ///<< Base right edge margin
    int m_marginTop;                ///<< Base top edge margin
    int m_marginBottom;             ///<< Base bottom edge margin
    int m_spacing;                  ///<< Base item spacing

    QLayout* m_pLayout;             ///< Wrapped object derived pointer

    /// Struct for base scaling values of spacer items
    struct SpacerItemWrapper
    {
        QSpacerItem* pItem;         ///< Spacer item pointer
        QSize size;                 ///< Base size of the spacer
    };

    QVector<SpacerItemWrapper> m_spacerArray;   ///< List of spacer items in the layout
};

#endif // _SCALED_LAYOUT_WRAPPER_H_
