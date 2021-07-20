//=============================================================================
/// Copyright (c) 2019-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QCheckBox object.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SCALED_CHECKBOX_H_
#define QTCOMMON_CUSTOM_WIDGETS_SCALED_CHECKBOX_H_

#include <QCheckBox>

/// Wraps the QCheckBox and fixes DPI scaling.
class ScaledCheckBox : public QCheckBox
{
    Q_OBJECT

public:

    /// Constructor which sets the parent of this object.
    /// \param parent The parent of this widget.
    ScaledCheckBox(QWidget* parent = nullptr);

    /// Constructor which sets the text of the checkbox and the parent object.
    /// \param text The initial text shown next to the checkbox.
    /// \param parent The parent of this widget.
    ScaledCheckBox(const QString& text, QWidget* parent = nullptr);

    /// Virtual destructor.
    virtual ~ScaledCheckBox();

    /// Update the indicator size to reflect the current font metrics.
    /// If the font is changed programmatically, this should be called immediately after.
    void UpdateIndicatorSize();

    /// Overridden to return the scaled size hint for the QCheckBox.
    /// \return The preferred size of this check box.
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

    /// Simple stylesheet template for updating the indicator size when the DPI scale changes.
    /// This ties the indicator size to the font size,
    /// and assumes a 1:1 width:height ratio of the indicator icons.
    const QString kIndicatorSizeStylesheet_ = "ScaledCheckBox::indicator { width: %1px; height: %1px; }";

protected:
    /// \brief Reimplement changeEvent to handle resizing on fontChange.
    /// \param event The event that caused the change.
    virtual void changeEvent(QEvent* event) override;

private slots:
    /// Handler for ScaleFactorChanged signals.
    /// Invalidates font sizes and resizes the checkbox indicator.
    void OnScaleFactorChanged();

private:
    /// Track if invalidating font metrics so the changeEvent does not become recursive.
    bool invalidating_font_metrics_;
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SCALED_CHECKBOX_H_
