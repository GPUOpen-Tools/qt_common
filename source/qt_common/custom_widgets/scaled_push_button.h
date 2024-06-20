//=============================================================================
/// Copyright (c) 2019-2021 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Scaling wrapper for QPushButton object.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SCALED_PUSHBUTTON_H_
#define QTCOMMON_CUSTOM_WIDGETS_SCALED_PUSHBUTTON_H_

#include <QPushButton>

/// @brief Wraps the QPushButton and fixes DPI scaling.
class ScaledPushButton : public QPushButton
{
    Q_OBJECT

public:
    /// @brief Constructor.
    ///
    /// @param parent The parent object.
    ScaledPushButton(QWidget* parent = nullptr);

    /// @brief Constructor.
    ///
    /// @param text The button text.
    /// @param parent The parent object.
    ScaledPushButton(const QString& text, QWidget* parent = nullptr);

    /// @brief Destructor.
    virtual ~ScaledPushButton();

    /// @brief Sets the correct link button stylesheet based on the current color theme and connects a slot to update if color theme is changed.
    void SetLinkStyleSheet();

private slots:
    /// @brief Callback for when the DPI scale factor changes.
    void OnScaleFactorChanged();

    /// @brief Slot function to set the correct link button stylesheet based on the current color theme when the color theme is changed.
    void UpdateLinkButtonStyleSheet();

};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SCALED_PUSHBUTTON_H_
