//=============================================================================
/// Copyright (c) 2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QFontComboBox object.
//=============================================================================

#ifndef QTCOMMON_CUSTOMWIDGETS_SCALED_FONT_COMBO_BOX_H_
#define QTCOMMON_CUSTOMWIDGETS_SCALED_FONT_COMBO_BOX_H_

#include <QFontComboBox>

/// Wraps the QComboBox and fixes DPI scaling.
class ScaledFontComboBox : public QFontComboBox
{
    Q_OBJECT

public:
    /// Constructor.
    /// \param parent the parent object
    ScaledFontComboBox(QWidget* parent = nullptr);

    /// Destructor.
    virtual ~ScaledFontComboBox();

private slots:
    /// Callback for when the DPI scale factor changes
    void OnScaleFactorChanged();
};

#endif  // QTCOMMON_CUSTOMWIDGETS_SCALED_FONT_COMBO_BOX_H_
