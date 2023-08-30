//=============================================================================
/// Copyright (c) 2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QSpinBox object.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SCALED_SPIN_BOX_H_
#define QTCOMMON_CUSTOM_WIDGETS_SCALED_SPIN_BOX_H_

#include <QSpinBox>

/// Wraps the QSpinBox and fixes DPI scaling.
class ScaledSpinBox : public QSpinBox
{
    Q_OBJECT

public:
    /// Constructor.
    /// \param parent The parent object.
    ScaledSpinBox(QWidget* parent = nullptr);

    /// Destructor.
    virtual ~ScaledSpinBox();

    /// Overridden sizeHint that adjusts the height
    /// of the spinbox to match a DPI scaled font.
    /// \return The preferred size for the widget.
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

private slots:
    /// Callback for when the DPI scale factor changes.
    void OnScaleFactorChanged();
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SCALED_SPIN_BOX_H_
