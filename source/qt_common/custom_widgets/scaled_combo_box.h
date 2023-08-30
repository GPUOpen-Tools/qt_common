//=============================================================================
/// Copyright (c) 2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QComboBox object.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SCALED_COMBO_BOX_H_
#define QTCOMMON_CUSTOM_WIDGETS_SCALED_COMBO_BOX_H_

#include <QComboBox>

/// Wraps the QComboBox and fixes DPI scaling.
class ScaledComboBox : public QComboBox
{
    Q_OBJECT

public:
    /// Constructor.
    /// \param parent the parent object
    ScaledComboBox(QWidget* parent = nullptr);

    /// Destructor.
    virtual ~ScaledComboBox();

private slots:
    /// Callback for when the DPI scale factor changes
    void OnScaleFactorChanged();
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SCALED_COMBO_BOX_H_
