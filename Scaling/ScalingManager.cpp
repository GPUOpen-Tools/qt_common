//=============================================================================
/// Copyright (c) 2016-2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of the ScalingManager. The ScalingManager handles
/// scaling operations such as providing the current scale factor and emitting
/// the ScaleFactorChanged signal for other QObjects to utilize. It can also
/// manage and scale any registered objects automatically, to the best of its
/// ability.
//=============================================================================

#include <ScalingManager.h>

#include <QApplication>
#include <QScreen>
#include <QWindow>
#include <QLayout>
#include <QWidget>
#include <QDebug>
#include <ScaledObjectWrapper.h>
#include <ScaledWidgetWrapper.h>
#include <ScaledLayoutWrapper.h>
#include <ScaledGridLayoutWrapper.h>
#include <ScaledMainWindowWrapper.h>

#ifndef __APPLE__
const static int s_STANDARD_DPI = 96;
#endif
const static QString s_STYLESHEET_SCALING_REGEX = "(-?\\d+)px\\b"; // Regex which matches a number followed by "px", such as: 6px, -32px, 200px, etc.
const static char* s_BLOCK_SCALING_PROPERTY_NAME = "blockScaling";

//-----------------------------------------------------------------------------
/// Constructor.
//-----------------------------------------------------------------------------
ScalingManager::ScalingManager()
{
#ifdef __APPLE__
    // MacOSX handles scaling - so force to 1.0
    m_scaleFactor = 1.0;
#else
    m_dpi = QApplication::primaryScreen()->logicalDotsPerInch();
    m_scaleFactor = (double)m_dpi / (double)s_STANDARD_DPI;
#endif
    m_rescaleFactor = 1;
}

//-----------------------------------------------------------------------------
/// Destructor.
//-----------------------------------------------------------------------------
ScalingManager::~ScalingManager()
{
}

//-----------------------------------------------------------------------------
/// ScalingManager instance get function.
/// \return a reference to the ScalingManager instance.
//-----------------------------------------------------------------------------
ScalingManager& ScalingManager::Get()
{
    static ScalingManager s_Instance;
    return s_Instance;
}

//-----------------------------------------------------------------------------
/// Initialize the scaing manager with the given main window (this window will
/// signal all changes in dpi).
/// \pMainWindow The application main window.
//-----------------------------------------------------------------------------
void ScalingManager::Initialize(QMainWindow* pMainWindow)
{
    m_pMainWindow = pMainWindow;
    QWindow* pWinHandle = pMainWindow->windowHandle();

    // Screen DPI changed signals/slots
    for (QScreen* pScreen : QApplication::screens())
    {
        connect(pScreen, &QScreen::logicalDotsPerInchChanged, this, &ScalingManager::Update);
    }

    // Screen changed and added signals/slots
    connect(pWinHandle, &QWindow::screenChanged, this, &ScalingManager::OnScreenChanged);
    connect(qApp, &QApplication::screenAdded, this, &ScalingManager::OnScreenAdded);

    // DPI update for current screen
    if (pWinHandle != nullptr && pWinHandle->screen() != nullptr)
    {
        Update(pWinHandle->screen()->logicalDotsPerInch());
    }
}

//-----------------------------------------------------------------------------
/// Scaling update slot - called anytime that the dpi might have changed. If a
/// dpi change occurs the scale factor is calculated and the scaleFactorChanged
/// signal is emitted.
//-----------------------------------------------------------------------------
void ScalingManager::Update(qreal dpi)
{
    QObject* pSenderObject = sender();
    QScreen* pMonitorOfSender = qobject_cast<QScreen*>(pSenderObject);
    QWindow* pWinHandle = m_pMainWindow->windowHandle();
    QScreen* pMonitorOfMainWindow = pWinHandle->screen();

    if (!pMonitorOfSender || (pMonitorOfMainWindow == pMonitorOfSender))
    {

        // If dpi changes, trigger a scale factor change
        if (m_dpi != dpi)
        {
            m_dpi = dpi;
            double oldScaleFactor = m_scaleFactor;
#ifdef __APPLE__
            // MacOSX handles scaling - so force to 1.0
            m_scaleFactor = 1.0;
#else
            m_scaleFactor = (double)m_dpi / (double)s_STANDARD_DPI;
#endif
            m_rescaleFactor = m_scaleFactor / oldScaleFactor;

            ApplyScaling();

            // @TODO - Change this signal so it outputs the scale factor and the rescale factor
            // (the rescale factor is usually a more useful value than the old scale factor)
            emit ScalingManager::ScaleFactorChanged(oldScaleFactor, m_scaleFactor);
        }
    }
}

//-----------------------------------------------------------------------------
/// Get the current scale factor.
/// \return The current UI scale factor.
//-----------------------------------------------------------------------------
double ScalingManager::GetScaleFactor() const
{
    return m_scaleFactor;
}

//-----------------------------------------------------------------------------
/// Get the rescale factor. This represents the factor of change for the dpi
/// itself (calculated as newScaleFactor / oldScaleFactor). This is used to
/// modify the size of components as part of a scaling operation without needing
/// to use some base size (such as resizing the main window when the dpi changes).
///
/// Rescaling should generally only be used with components that have dynamic
/// sizes (usually anything the user can manually resize). Fixed size components
/// should try to use normal scaling instead.
/// \return The rescale factor from the most recent dpi change.
//-----------------------------------------------------------------------------
double ScalingManager::GetRescaleFactor() const
{
    return m_rescaleFactor;
}

//-----------------------------------------------------------------------------
/// Integer scale function - scale an integer value by the scale factor.
/// \param i The integer to scale.
/// \return The scaled integer.
//-----------------------------------------------------------------------------
int ScalingManager::Scaled(int i) const
{
    return i * m_scaleFactor;
}

//-----------------------------------------------------------------------------
/// Double scale function - scale an double value by the scale factor.
/// \param d The double to scale.
/// \return The double integer.
//-----------------------------------------------------------------------------
double ScalingManager::Scaled(double d) const
{
    return d * m_scaleFactor;
}

//-----------------------------------------------------------------------------
/// QSize scale function - scale an QSize by the scale factor.
/// \param qs The QSize to scale.
/// \return The scaled QSize.
//-----------------------------------------------------------------------------
QSize ScalingManager::Scaled(const QSize& qs) const
{
    return QSize(qs.width() * m_scaleFactor, qs.height() * m_scaleFactor);
}

//-----------------------------------------------------------------------------
/// QRect scale function - scale an QRect by the scale factor.
/// \param qr The QRect to scale.
/// \return The scaled QRect.
//-----------------------------------------------------------------------------
QRect ScalingManager::Scaled(const QRect& qr) const
{
    return QRect(qr.x() * m_scaleFactor, qr.y() * m_scaleFactor, qr.width() * m_scaleFactor, qr.height() * m_scaleFactor);
}

//-----------------------------------------------------------------------------
/// Integer rescale function - rescale an integer value using the rescale factor.
/// \param i The integer to rescale.
/// \return The rescaled integer.
//-----------------------------------------------------------------------------
int ScalingManager::Rescaled(int i) const
{
    return i * m_rescaleFactor;
}

//-----------------------------------------------------------------------------
/// Double rescale function - rescale an Double value using the rescale factor.
/// \param d The double to rescale.
/// \return The rescaled double.
//-----------------------------------------------------------------------------
double ScalingManager::Rescaled(double d) const
{
    return d * m_rescaleFactor;
}

//-----------------------------------------------------------------------------
/// QSize rescale function - rescale an QSize using the rescale factor.
/// \param qs The QSize to rescale.
/// \return The rescaled QSize.
//-----------------------------------------------------------------------------
QSize ScalingManager::Rescaled(const QSize& qs) const
{
    return QSize(qs.width() * m_rescaleFactor, qs.height() * m_rescaleFactor);
}

//-----------------------------------------------------------------------------
/// QRect rescale function - rescale an QRect using the rescale factor.
/// \param qr The QRect to rescale.
/// \return The rescaled QRect.
//-----------------------------------------------------------------------------
QRect ScalingManager::Rescaled(const QRect& qr) const
{
    return QRect(qr.x() * m_rescaleFactor, qr.y() * m_rescaleFactor, qr.width() * m_rescaleFactor, qr.height() * m_rescaleFactor);
}

//-----------------------------------------------------------------------------
/// Get a scaled copy of a stylesheet.
/// \param styleSheet The base stylesheet.
/// \return A stylesheet which has been scaled using the current scale factor.
//-----------------------------------------------------------------------------
QString ScalingManager::ScaledStylesheet(const QString& styleSheet) const
{
    QString scaledStylesheet = styleSheet;

    // Scale any stylesheet pixel values
    if (scaledStylesheet.compare("") != 0)
    {
        QRegExp regex(s_STYLESHEET_SCALING_REGEX);

        int lastIndex = 0;
        int scaledLength = 0;

        // Iterate through length matches (every place in fomat ##px)
        while ((lastIndex = regex.indexIn(scaledStylesheet, lastIndex)) != -1)
        {
            // Calculate scaled length from base length (pulled from regex)
            scaledLength = regex.cap(1).toInt() * m_scaleFactor;

            // Replace the current matched length with the scaled length
            QString newSizeStr = QString::number(scaledLength) + QString("px");
            scaledStylesheet.replace(lastIndex, regex.matchedLength(), newSizeStr);

            lastIndex += newSizeStr.size();
        }
    }

    return scaledStylesheet;
}

//-----------------------------------------------------------------------------
/// Check if the given stylesheet has components which need to be scaled. This
/// is used to optimize scaling operations by ignoring stylesheets which don't
/// require any scaling.
/// \param styleSheet The stylesheet to check.
/// \return True if the stylesheet has scalable components, false otherwise.
//-----------------------------------------------------------------------------
bool ScalingManager::IsStylesheetScalingRequired(const QString& styleSheet)
{
    QRegExp regex(s_STYLESHEET_SCALING_REGEX);

    // Utilize the same regex from the ScaledStylesheet function and just verify there is at least one match
    return (regex.indexIn(styleSheet) != -1);
}

//-----------------------------------------------------------------------------
/// Create a scaled wrapper for this object. If the object is not of a scalable
/// type then nullptr is returned
/// \param pObject The Qt object to wrap with a scaled wrapper.
/// \return A pointer to the object wrapper.
//-----------------------------------------------------------------------------
ScaledObjectWrapper* ScalingManager::CreateObjectWrapper(QObject* pObject) const
{
    // Note: order is important here - derived wrappers must appear
    // before their base wrapper classes to ensure the correct wrapper is returned

    // Determine object derived class
    if (pObject->inherits("QMainWindow"))
    {
        return new ScaledMainWindowWrapper(pObject);
    }
    else if (pObject->isWidgetType())
    {
        return new ScaledWidgetWrapper(pObject);
    }
    else if (pObject->inherits("QGridLayout"))
    {
        return new ScaledGridLayoutWrapper(pObject);
    }
    else if (pObject->inherits("QLayout") && !pObject->inherits("QMainWindowLayout"))
    {
        return new ScaledLayoutWrapper(pObject);
    }

    // Return null for classes without scaled wrappers
    return nullptr;
}

//-----------------------------------------------------------------------------
/// Get the scaled wrapper for the given object. If the object has no wrapper
/// then nullptr is returned.
/// \param pObject The Qt object to get the wrapper of.
/// \return A pointer to the object wrapper.
//-----------------------------------------------------------------------------
ScaledObjectWrapper* ScalingManager::GetObjectWrapper(QObject* pObject) const
{
    // Get scale info from object pointer (maybe make this a function)
    for (ScaledObjectWrapper* pWrapper : m_registeredObjectArray)
    {
        if (pWrapper->GetWrappedObject() == pObject)
        {
            return pWrapper;
        }
    }

    // Return null if no wrapper was found for this object
    return nullptr;
}

//-----------------------------------------------------------------------------
/// Check whether an object is already registered with the scaling manager.
/// \param pObject The object that is being checked.
/// \return true if the object is registered, false otherwise.
//-----------------------------------------------------------------------------
bool ScalingManager::IsRegisteredObject(QObject* pObject) const
{
    return GetObjectWrapper(pObject) != nullptr;
}

//-----------------------------------------------------------------------------
/// Registers a widget for scaling using it's current state as the base (appearance
/// at 100% scaling).
/// \param pObject The object to register.
/// \param flags The object registration flags to use. The flags are as follows:
///
///    REGISTER_CHILDREN - Register all children of this object as well
///    OVERRIDE_PREVIOUS - Override the previous base values if the object is already registered
///    APPLY_INITIAL_SCALE - Apply initial scaling to the object once it is registered
///    BREAK_ON_FAILURE - Program execution will break if registration fails
///
/// Note: Flags are passed down to children when they are registered, so if the
/// REGISTER_CHILDREN flag is set it will recursively register all children of
/// those children too (and so on).
///
/// Note: Because of the nature of recursively adding children, the BREAK_ON_FAILURE
/// flag is disabled for child objects when registering children. This is because
/// it is likely that somewhere in the qt heirarchy a child is either nullptr or an
/// unscalable object, and we just need to ignore these objects rather than breaking.
//-----------------------------------------------------------------------------
void ScalingManager::RegisterObject(QObject* pObject, int flags)
{
    // Block scaling property used to control which panels get registered
    // @TODO: Achieve this without using dynamic qt properties - probably cleaner to just maintain a list of blocked objects
    if (pObject->property(s_BLOCK_SCALING_PROPERTY_NAME).isValid() &&
        pObject->property(s_BLOCK_SCALING_PROPERTY_NAME).toBool() == true)
    {
        return;
    }

    // Do nothing with a null pointer
    if (pObject == nullptr)
    {
        Q_ASSERT((flags & BREAK_ON_FAILURE) == false);
        return;
    }

    ScaledObjectWrapper* pWrapper = nullptr;

    // If wrapper is not found, then this object is unregistered and a new wrapper
    // needs to be created. If the wrapper is found then use the existing wrapper.
    pWrapper = GetObjectWrapper(pObject);
    if (pWrapper == nullptr)
    {
        pWrapper = CreateObjectWrapper(pObject);

        // If the widget isn't of a registerable type then return
        if (pWrapper == nullptr)
        {
            Q_ASSERT((flags & BREAK_ON_FAILURE) == false);
            return;
        }

        pWrapper->SetCurrentAsBase();
        m_registeredObjectArray.push_back(pWrapper);

        // Object destruction signal/slot
        connect(pObject, &QObject::destroyed, this, &ScalingManager::OnObjectDestroyed);
    }
    else if (flags & OVERRIDE_PREVIOUS)
    {
        pWrapper->SetCurrentAsBase();
    }

    // Register child objects
    if ((flags & REGISTER_CHILDREN))
    {
        for (QObject* pChild : pObject->children())
        {
            RegisterObject(pChild, flags & (~BREAK_ON_FAILURE));
        }
    }

    // Initial scaling
    if (flags & APPLY_INITIAL_SCALE && pWrapper != nullptr)
    {
        pWrapper->ApplyScaling();
    }
}

//-----------------------------------------------------------------------------
/// Unregisters an object for scaling. This means the object will no longer
/// have any scaling operations performed by the scaling manager. This is called
/// automatically on registered objects just before they are destroyed, so
/// in most cases this doesn't need to be called.
/// \param pObject The object to unregister.
//-----------------------------------------------------------------------------
void ScalingManager::UnregisterObject(QObject* pObject)
{
    // Do nothing if the widget is already unregistered
    if (!IsRegisteredObject(pObject))
    {
        return;
    }

    // Delete base info object
    ScaledObjectWrapper* pWrapper = GetObjectWrapper(pObject);
    delete pWrapper;

    // Remove widget from list
    m_registeredObjectArray.removeAll(pWrapper);
}

//-----------------------------------------------------------------------------
/// Register all existing objects which can be registered. This is implemented
/// by registering all top level widgets and recursively registering all their
/// children.
//-----------------------------------------------------------------------------
void ScalingManager::RegisterAll()
{
    // Register all top level widgets and their children (should recursively reach all scalable objects)
    for (QObject* pObject : QApplication::topLevelWidgets())
    {
        RegisterObject(pObject, REGISTER_CHILDREN | APPLY_INITIAL_SCALE);
    }
}

//-----------------------------------------------------------------------------
/// Apply the current scale factor to all managed objects. This is a little
/// slow, mostly due to stylesheet scaling which utilizes regex, so it should
/// be called infrequently, at a time when it is acceptable for the program to
/// pause briefly. In general, this should only need to be called any time the
/// scale factor changes. If adding new objects dynamically during user operation
/// this doesn't need to be called, as newly registered objects are automatically
/// scaled when they get registered.
//-----------------------------------------------------------------------------
void ScalingManager::ApplyScaling() const
{
    // Scale all registered widgets
    for (ScaledObjectWrapper* pWrapper : m_registeredObjectArray)
    {
        pWrapper->ApplyScaling();
    }
}

//-----------------------------------------------------------------------------
/// Disable scaling for the given object. This prevents the object from being
/// registered with the scaling manager at any point. If the object is already
/// registered, it will be unregistered.
///
/// Note: This will prevent children of the object from being registered via the
/// hierarchy descending from the object (so none of the descendants will get
/// registered when calling RegisterAll), however the children can still be accessed
/// and registered manually if needed.
/// \param pObject The object to disable scaling for.
//-----------------------------------------------------------------------------
void ScalingManager::DisableScalingForObject(QObject* pObject)
{
    pObject->setProperty(s_BLOCK_SCALING_PROPERTY_NAME, true);
    UnregisterObject(pObject);
}

//-----------------------------------------------------------------------------
/// Enable scaling for the given object. This allows the object to be registered
/// with the scaling manager if it wasn't able to previously (by default all
/// objects are scaling-enabled).
/// \param pObject The object to enable scaling for.
//-----------------------------------------------------------------------------
void ScalingManager::EnableScalingForObject(QObject* pObject)
{
    pObject->setProperty(s_BLOCK_SCALING_PROPERTY_NAME, false);
}

//-----------------------------------------------------------------------------
/// Object destruction slot - called on registered objects when they are destroyed
/// to make sure they get unregistered.
/// \param pObject The object being destroyed
//-----------------------------------------------------------------------------
void ScalingManager::OnObjectDestroyed(QObject *pObject)
{
    UnregisterObject(pObject);
}

//-----------------------------------------------------------------------------
/// Screen changed slot - triggers a DPI update when the screen the main window
/// is in changes.
/// \param pScreen The new screen the main window is in.
//-----------------------------------------------------------------------------
void ScalingManager::OnScreenChanged(QScreen* pScreen)
{
    // DPI update
    Update(pScreen->logicalDotsPerInch());
}

//-----------------------------------------------------------------------------
/// Screen added slot - Connects appropriate signals/slots for new screens when
/// they are added.
/// \param pScreen The newly added screen.
//-----------------------------------------------------------------------------
void ScalingManager::OnScreenAdded(QScreen* pScreen)
{
    // Connect up signals/slots for new screen
    connect(pScreen, &QScreen::logicalDotsPerInchChanged, this, &ScalingManager::Update);
}

//-----------------------------------------------------------------------------
/// Helper function to add a widget to the layout of another widget and register
/// the widget immediately after.
/// \param pWidget The new widget being added.
/// \param pParentWidget The parent widget being added to.
//-----------------------------------------------------------------------------
void ScalingManager::RegisterAndAddToWidget(QWidget* pWidget, QWidget* pParentWidget)
{
    pParentWidget->layout()->addWidget(pWidget);
    RegisterObject(pWidget);
}
