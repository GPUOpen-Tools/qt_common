//=============================================================================
/// Copyright (c) 2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QGroupBox object.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SCALED_GROUPBOX_H_
#define QTCOMMON_CUSTOM_WIDGETS_SCALED_GROUPBOX_H_

#include <QGroupBox>

/// Wraps the QGroupBox and fixes DPI scaling.
class ScaledGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    /// Constructor.
    /// \param parent the parent object
    ScaledGroupBox(QWidget* parent = nullptr);

    /// Constructor.
    /// \param title the box title
    /// \param parent The parent object
    ScaledGroupBox(const QString& title, QWidget* parent = nullptr);

    /// Destructor.
    virtual ~ScaledGroupBox();

private slots:
    /// Callback for when the DPI scale factor changes
    void OnScaleFactorChanged();
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SCALED_GROUPBOX_H_
