//=============================================================================
/// Copyright (c) 2019-2025 Advanced Micro Devices, Inc. All rights reserved.
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

    /// \brief Override the show event to invalidate the font metrics when first shown.
    /// \param show_event The QShowEvent.
    void showEvent(QShowEvent* show_event) Q_DECL_OVERRIDE;

private:
    /// Track if invalidating font metrics so the changeEvent does not become recursive.
    bool invalidating_font_metrics_;

    bool first_show_;  ///< Boolean to track the first time this widget is shown to invalidate the font metrics.
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SCALED_LABEL_H_
