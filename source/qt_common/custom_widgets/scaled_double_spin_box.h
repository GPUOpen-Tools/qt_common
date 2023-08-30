//=============================================================================
/// Copyright (c) 2022 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QSpinBox object.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SCALED_DOUBLE_SPIN_BOX_H_
#define QTCOMMON_CUSTOM_WIDGETS_SCALED_DOUBLE_SPIN_BOX_H_

#include <QDoubleSpinBox>

/// Wraps the QSpinBox and fixes DPI scaling.
class ScaledDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    /// Constructor.
    /// \param parent The parent object.
    ScaledDoubleSpinBox(QWidget* parent = nullptr);

    /// Destructor.
    virtual ~ScaledDoubleSpinBox();

    /// Overridden sizeHint that adjusts the height
    /// of the spinbox to match a DPI scaled font.
    /// \return The preferred size for the widget.
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

private slots:
    /// Callback for when the DPI scale factor changes.
    void OnScaleFactorChanged();
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SCALED_DOUBLE_SPIN_BOX_H_
