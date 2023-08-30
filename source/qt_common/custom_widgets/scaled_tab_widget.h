//=============================================================================
/// Copyright (c) 2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QTabWidget object.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SCALED_TAB_WIDGET_H_
#define QTCOMMON_CUSTOM_WIDGETS_SCALED_TAB_WIDGET_H_

#include <QTabWidget>

/// Wraps the QTabWidget and fixes DPI scaling.
class ScaledTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    /// Constructor.
    /// \param parent The parent object.
    ScaledTabWidget(QWidget* parent = nullptr);

    /// Destructor.
    virtual ~ScaledTabWidget();

private slots:
    /// Callback for when the DPI scale factor changes.
    void OnScaleFactorChanged();
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SCALED_TAB_WIDGET_H_
