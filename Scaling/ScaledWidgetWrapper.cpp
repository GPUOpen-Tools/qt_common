//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QWidget object. The functionality for the
/// various things that need to be scaled (sizes, fonts, stylesheets, etc.) is
/// broken into a few functions to make it easier to use/not use those scaling
/// operations in derived wrapper classes.
//=============================================================================

#include <ScaledWidgetWrapper.h>
#include <ScalingManager.h>

#include <QWidget>
#include <QSize>
#include <QPushButton>
#include <QVariant>
#include <QLayout>

static const QFont s_DUMMY_FONT("dummy");

//-----------------------------------------------------------------------------
/// Constructor.
/// \param pObject The object this class is wrapping.
//-----------------------------------------------------------------------------
ScaledWidgetWrapper::ScaledWidgetWrapper(QObject* pObject) :
    ScaledObjectWrapper(pObject),
    m_pWidget(static_cast<QWidget*>(pObject))
{
}

//-----------------------------------------------------------------------------
/// Destructor.
//-----------------------------------------------------------------------------
ScaledWidgetWrapper::~ScaledWidgetWrapper()
{
}

//-----------------------------------------------------------------------------
/// Apply scaling for this widget using the stored base scaling values.
//-----------------------------------------------------------------------------
void ScaledWidgetWrapper::ApplyScaling()
{
    ScaleMaxAndMinSize();
    ScaleGeometry();
    ScaleFont();
    ScaleStylesheet();
}

//-----------------------------------------------------------------------------
/// Store the base scaling values for this widget. This stores select data from the
/// widget in it's current state to be used when applying scaling. The widget's
/// base values determine the widget's appearance at 100% scaling.
//-----------------------------------------------------------------------------
void ScaledWidgetWrapper::SetCurrentAsBase()
{
    m_doFontPixelScaling = false;
    m_doScaleStylesheet = false;

    // Get widget values
    m_maxSize = m_pWidget->maximumSize();
    m_minSize = m_pWidget->minimumSize();
    m_geometry = m_pWidget->geometry();

    // Determine if pixel level font scaling is required (fonts defined with point sizes will scale automatically)
    if (m_pWidget->font().pixelSize() != -1)
    {
        m_font = m_pWidget->font();
        m_doFontPixelScaling = true;
    }

    // Determine if stylesheet scaling is required
    if (ScalingManager::Get().IsStylesheetScalingRequired(m_pWidget->styleSheet()))
    {
        m_stylesheet = m_pWidget->styleSheet();
        m_doScaleStylesheet = true;
    }
}

//-----------------------------------------------------------------------------
/// Scale the maximum and minimum size of this widget. This will only scale the
/// max/min if it is not being set already by the layout size constraint.
//-----------------------------------------------------------------------------
void ScaledWidgetWrapper::ScaleMaxAndMinSize()
{
    ScalingManager& sm = ScalingManager::Get();

    QLayout* pLayout = m_pWidget->layout();
    QLayout::SizeConstraint sc = (pLayout != nullptr) ? pLayout->sizeConstraint() : QLayout::SetNoConstraint;

    // Determine whether to scale max/min size based on layout constraints
    bool doScaleMin = (sc != QLayout::SetMinimumSize) && (sc != QLayout::SetMinAndMaxSize);
    bool doScaleMax = (sc != QLayout::SetMaximumSize) && (sc != QLayout::SetMinAndMaxSize);

    // Only scale min if not determined by layout
    if (doScaleMin)
    {
        m_pWidget->setMinimumSize(sm.Scaled(m_minSize).expandedTo(QSize(0, 0)));
    }

    // Only scale max if not determined by layout
    if (doScaleMax)
    {
        m_pWidget->setMaximumSize(sm.Scaled(m_maxSize).boundedTo(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX)));
    }

    // @TODO - When the constraint is SetDefaultConstraint, ideally we need to check
    // if the min size has been explicitly set, or whether it gets set by the
    // layout constraint. If explicit, we should scale it, but if not then it should
    // be left alone. There is currently no way to determine whether this value has
    // been set explicitly or not because the explicitMinSize variable in QWidget is
    // kept private, with no way to access it short of using the private qt headers.
}

//-----------------------------------------------------------------------------
/// Scale the geometry of this widget.
//-----------------------------------------------------------------------------
void ScaledWidgetWrapper::ScaleGeometry()
{
    ScalingManager& sm = ScalingManager::Get();

    m_pWidget->setGeometry(sm.Scaled(m_geometry));
}

//-----------------------------------------------------------------------------
/// Scale the font for this widget.
//-----------------------------------------------------------------------------
void ScaledWidgetWrapper::ScaleFont()
{
    ScalingManager& sm = ScalingManager::Get();

    // Use pixel font scaling if necessary, otherwise just trigger a font refresh
    if (m_doFontPixelScaling)
    {
        QFont temp = m_font;
        temp.setPixelSize(m_font.pixelSize() * sm.GetScaleFactor());
        m_pWidget->setFont(temp);
    }
    else
    {
        // Re-apply font after applying a dummy font to trigger scaling when using point-size fonts
        QFont temp = m_pWidget->font();
        m_pWidget->setFont(s_DUMMY_FONT);
        m_pWidget->setFont(temp);
    }
}

//-----------------------------------------------------------------------------
/// Scale the stylesheet for this widget.
//-----------------------------------------------------------------------------
void ScaledWidgetWrapper::ScaleStylesheet()
{
    ScalingManager& sm = ScalingManager::Get();

    // Scale the stylesheet if necessary
    if (m_doScaleStylesheet)
    {
        m_pWidget->setStyleSheet(sm.ScaledStylesheet(m_stylesheet));
    }
}
