//=============================================================================
/// Copyright (c) 2020-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for a QLabel derived widget that truncates the string to
/// the width of the longest widget in the same layout.
///
/// This widget handles the case where a label needs to display text within
/// a layout, but not cause the layout to expand horizontally if the label text
/// is unable to fit.  Instead, an ellipsis is added to the end of the label
/// text and a tooltip for the label displays the full text.  In this scenario,
/// set the size hint policy for the width to be QSizePolicy::Ignore.
///
/// To have this label widget expand to use all available width in a layout
/// without expanding the layout width any further, set the size hint policy
/// for the width to be QSizePolicy::MinimumExpanding.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_ELIDED_LINE_LABEL_H_
#define QTCOMMON_CUSTOM_WIDGETS_ELIDED_LINE_LABEL_H_

#include "scaled_label.h"

/// Reimplements the QLabel widget that limits the width
/// of the text.
class ElidedLineLabel : public ScaledLabel
{
    Q_OBJECT

    Q_PROPERTY(QString text READ text WRITE setText)

public:
    /// The constructor for the ElidedLineLabel.
    /// \param parent A pointer to the parent widget.
    ElidedLineLabel(QWidget* parent);

    /// The destructor for the ElidedLineLabel.
    virtual ~ElidedLineLabel();

    /// Set text string for the label and also set the tooltip to the
    /// full text string if the label needs to be truncated.
    /// \param text_string The full text string to use for the label.
    void setText(const QString& text_string);

    /// Retrieve the full text string for the label.
    /// \return The label's text string.
    QString text() const;

protected:
    /// Implementation of Qt's resizeEvent() method for this item.
    /// The text string is updated and truncated if necessary.
    /// \param event A pointer to the resize event.
    virtual void resizeEvent(QResizeEvent* event) override;

    /// Set the label text, truncating if necessary.  Also, update
    /// the tooltip text.
    void UpdateLabelText();

    /// Set the tooltip to the full text if the label text is
    /// truncated.  Otherwise clear the tooltip text.
    void UpdateToolTip();

private:
    QString full_text_;  ///< The full text string of the label (before being truncated).
};
#endif  // QTCOMMON_CUSTOM_WIDGETS_ELIDED_LINE_LABEL_H_
