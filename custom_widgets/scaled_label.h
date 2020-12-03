//=============================================================================
/// Copyright (c) 2019-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QLabel object.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SCALED_LABEL_H_
#define QTCOMMON_CUSTOM_WIDGETS_SCALED_LABEL_H_

#include <QLabel>

/// Reimplements QLabel to fix DPI scaling.
class ScaledLabel : public QLabel
{
    Q_OBJECT

public:
    /// Constructor
    /// \param parent The parent widget
    ScaledLabel(QWidget* parent);

    /// Virtual destructor
    virtual ~ScaledLabel();

protected:
    /// \brief Reimplement changeEvent to handle resizing on fontChange.
    /// \param event The event that caused the change.
    virtual void changeEvent(QEvent* event) override;

private slots:
    /// Callback for when the DPI scale factor changes
    void OnScaleFactorChanged();

private:
    /// Track if invalidating font metrics so the changeEvent does not become recursive.
    bool invalidating_font_metrics_;
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SCALED_LABEL_H_
