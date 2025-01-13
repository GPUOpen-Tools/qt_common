//=============================================================================
/// Copyright (c) 2016-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Declaration for the ScalingManager
//=============================================================================

#ifndef QTCOMMON_UTILS_SCALING_MANAGER_H_
#define QTCOMMON_UTILS_SCALING_MANAGER_H_

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QMainWindow>

/// Scaling manager implementation
class ScalingManager : public QObject
{
    Q_OBJECT

public:
    /// ScalingManager instance get function.
    /// \return a reference to the ScalingManager instance.
    static ScalingManager& Get();

#ifdef _DEBUG
    /// Gets the physical DPI of the primary monitor, or the monitor that the application window is on.
    /// \return The physical DPI of the active monitor.
    qreal GetPhysicalDpi();
#endif // _DEBUG

    /// Initialize the scaling manager with the given main widget (this window will
    /// signal all changes in dpi).
    /// \param main_widget The application main widget.
    void Initialize(QWidget* main_widget);

private:
    /// Constructor/destructor is private for singleton
    explicit ScalingManager();

    /// Destructor
    virtual ~ScalingManager();

    /// Main widget pointer
    QWidget* main_widget_;

    /// UI scale factor
    /// Useful when scaling from a known base size to a larger size according the DPI scale.
    /// An example might be a custom painted widget that should be 50x50 pixels when at 100% DPI
    /// and needs to get scaled larger at higher DPI scaling values; ie: 50*150% = 75x75 pixels at 150% DPI.
    /// And when going back down to 100% DPI, the original values can be used: ie: 50* 100% = 50x50 pixels at 100%.
    ///< An initial DPI Scale of 200% => scale_factor_ = 2.0;
    ///< Changing DPI Scale to 100% => scale_factor_ = 1.0;
    ///< Changing DPI Scale to 200% => scale_factor_ = 2.0;
    ///< Changing DPI Scale to 300% => scale_factor_ = 3.0;
    double scale_factor_;

    /// UI rescale factor
    /// Useful when re-scaling current values from the system rather than known base values.
    /// An example might be a widget that has an arbitrary user-specified pointsize font,
    /// where the initial pointsize is unknown and must be queried using font().pointSize().
    /// To go up to 150% DPI scale, the new font size would be equivalent to font.setPointSize(font.pointSize() * rescale_factor_),
    /// which would change a 10pt font size into a 15pt font. Thus, calling font.pointSize() would now return 15 instead of 10.
    /// In order to reduce from 150% back down to 100% DPI scaling, the current value off font.pointSize() needs to get scaled back down.
    /// The same line of code can be used to rescale down from the current value: font.setPointSize(font.pointSize() * rescale_factor_).
    ///< An initial DPI scale of 200% => rescale_factor_ = 1.0. (ie: there is no previous scale, so nothing changes).
    ///< Changing DPI scale to 100% => rescale_factor_ = 0.5 (ie: half of previous scale).
    ///< Changing DPI scale to 200% => rescale_factor_ = 2.0 (ie: double the previous scale).
    ///< Changing DPI scale to 300% => rescale_factor_ = 1.5 (ie: 1.5x the previous scale).
    double rescale_factor_;

    /// Current screen DPI
    double dpi_;

    /// Stores the initial DPI on startup for better scaling of fonts.
    double initial_dpi_;

signals:
    /// Emitted when the ScalingManager detects that the DPI scaling has changed.
    void ScaleFactorChanged();

private slots:
    /// Screen changed slot - triggers a DPI update when the screen the main window
    /// is in changes.
    /// \param screen The new screen the main window is in.
    void OnScreenChanged(QScreen* screen);

    /// Screen added slot - Connects appropriate signals/slots for new screens when
    /// they are added.
    /// \param screen The newly added screen.
    void OnScreenAdded(QScreen* screen);

    /// Screen removed slot - Disconnects appropriate signals/slots for the screen
    /// that was removed.
    /// \param screen The newly added screen.
    void OnScreenRemoved(QScreen* screen);

    /// DPI update slot - called anytime that the dpi might have changed. If a
    /// dpi change occurs the scale factor is calculated and the scaleFactorChanged
    /// signal is emitted.
    /// \param dpi The new DPI value
    void OnDpiChanged(qreal dpi);

private:
    /// Recalculates scale factors based on the DPI and notifies widgets of the change.
    /// \param dpi The new DPI value
    void UpdateScaleFactor(qreal dpi);
};

#endif  // QTCOMMON_UTILS_SCALING_MANAGER_H_
