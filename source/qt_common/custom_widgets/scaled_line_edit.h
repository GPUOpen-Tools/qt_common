//=============================================================================
/// Copyright (c) 2020-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QLineEdit object.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SCALED_LINE_EDIT_H_
#define QTCOMMON_CUSTOM_WIDGETS_SCALED_LINE_EDIT_H_

#include <QLineEdit>

/// Wraps the QLineEdit and fixes DPI scaling.
class ScaledLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    /// Constructor.
    /// \param parent the parent object
    ScaledLineEdit(QWidget* parent = nullptr);

    /// Constructor.
    /// \param title the box title
    /// \param parent The parent object
    ScaledLineEdit(const QString& title, QWidget* parent = nullptr);

    /// Destructor.
    virtual ~ScaledLineEdit();

private slots:
    /// Callback for when the DPI scale factor changes
    void OnScaleFactorChanged();
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SCALED_LINE_EDIT_H_
