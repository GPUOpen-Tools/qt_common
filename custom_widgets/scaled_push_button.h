//=============================================================================
/// Copyright (c) 2019-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QPushButton object.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SCALED_PUSHBUTTON_H_
#define QTCOMMON_CUSTOM_WIDGETS_SCALED_PUSHBUTTON_H_

#include <QPushButton>

/// Wraps the QPushButton and fixes DPI scaling.
class ScaledPushButton : public QPushButton
{
    Q_OBJECT

public:
    /// Constructor.
    /// \param parent The parent object.
    ScaledPushButton(QWidget* parent = nullptr);

    /// Constructor.
    /// \param text The button text.
    /// \param parent The parent object.
    ScaledPushButton(const QString& text, QWidget* parent = nullptr);

    /// Destructor.
    virtual ~ScaledPushButton();

private slots:
    /// Callback for when the DPI scale factor changes.
    void OnScaleFactorChanged();
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SCALED_PUSHBUTTON_H_
