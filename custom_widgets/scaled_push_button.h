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

    /// @brief Overridden setIconSize for the QAbstractButton.
    ///
    /// @param size Maximum size for the icon. Smaller icons will not be scaled up.
    void setIconSize(const QSize& size);

private slots:
    /// @brief Callback for when the DPI scale factor changes.
    void OnScaleFactorChanged();

private:
    QSize base_icon_size_;  ///< Base icon size DPI scaling will be calculated from.
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SCALED_PUSHBUTTON_H_
