//=============================================================================
/// Copyright (c) 2016-2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Declaration for the ScalingManager
//=============================================================================

#ifndef _SCALING_MANAGER_H_
#define _SCALING_MANAGER_H_

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QMainWindow>

enum RegisterFlags
{
    REGISTER_CHILDREN   = (1 << 0),
    OVERRIDE_PREVIOUS   = (1 << 1),
    APPLY_INITIAL_SCALE = (1 << 2),
    BREAK_ON_FAILURE    = (1 << 3)
};

// Forward declarations
class ScaledObjectWrapper;

/// Scaling manager implementation
class ScalingManager : public QObject
{
    Q_OBJECT

public:
    static ScalingManager& Get();

    void Initialize(QMainWindow* pMainWindow);

    // Unmanaged scaling operations
    double GetScaleFactor() const;
    double GetRescaleFactor() const;
    int Scaled(int i) const;
    double Scaled(double d) const;
    QSize Scaled(const QSize& qs) const;
    QRect Scaled(const QRect& qr) const;
    int Rescaled(int i) const;
    double Rescaled(double d) const;
    QSize Rescaled(const QSize& qs) const;
    QRect Rescaled(const QRect& qr) const;
    QString ScaledStylesheet(const QString& styleSheet) const;
    static bool IsStylesheetScalingRequired(const QString& styleSheet);

    // Managed scaling operations
    bool IsRegisteredObject(QObject* pObject) const;
    void RegisterObject(QObject* pObject, int flags = REGISTER_CHILDREN | APPLY_INITIAL_SCALE);
    void UnregisterObject(QObject* pObject);
    void RegisterAll();
    void ApplyScaling() const;
    void DisableScalingForObject(QObject* pObject);
    void EnableScalingForObject(QObject* pObject);

    // Simultaneous register+add helper functions
    void RegisterAndAddToWidget(QWidget* pWidget, QWidget* pParentWidget);

private:
    // Constructor/destructor is private for singleton
    explicit ScalingManager();
    virtual ~ScalingManager();

    ScaledObjectWrapper* GetObjectWrapper(QObject* pObject) const;
    ScaledObjectWrapper* CreateObjectWrapper(QObject* pObject) const;

    QMainWindow* m_pMainWindow;                             ///< Main window pointer
    double m_scaleFactor;                                   ///< UI scale factor
    double m_rescaleFactor;                                 ///< Factor of change for the dpi/scale factor (calculated as currentScaleFactor / previousScaleFactor)
    double m_dpi;                                           ///< Screen DPI
    QVector<ScaledObjectWrapper*> m_registeredObjectArray;  ///< Registered object array - objects managed by the scaling manager

signals:
    void ScaleFactorChanged(double oldScaleFactor, double newScaleFactor);

private slots:
    void OnObjectDestroyed(QObject *pObject);
    void OnScreenChanged(QScreen* pScreen);
    void OnScreenAdded(QScreen* pScreen);
    void Update(qreal dpi);
};

#endif // _SCALING_MANAGER_H_