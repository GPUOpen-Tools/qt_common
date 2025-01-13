//=============================================================================
/// Copyright (c) 2016-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for a check box widget.
//=============================================================================
#ifndef QTCOMMON_CUSTOM_WIDGETS_CHECK_BOX_WIDGET_H_
#define QTCOMMON_CUSTOM_WIDGETS_CHECK_BOX_WIDGET_H_

#include <QCheckBox>
#include <QObject>

/// Custom CheckBox which handles DPI scale changes, and assumes
/// a 2:1 width:height ratio for the indicator. The size of the
/// indicator will be based on the font size.
class CheckBoxWidget : public QCheckBox
{
    Q_OBJECT

    // This custom check box overrides the behavior of the associated text label.
    // The standard setText() method is used to change the state of the checkbox if the
    // text matches the ON or OFF text strings.  In order to use the default behavior
    // of the QCheckBox, set the "label_text" property to the desired string.  This
    // will change the ON and OFF text strings to the "label_text" value.
    Q_PROPERTY(QString label_text READ text WRITE SetLabelText);

public:
    /// Explicit constructor
    /// \param parent The checkbox's parent
    explicit CheckBoxWidget(QWidget* parent = 0);

    /// Virtual Destructor
    virtual ~CheckBoxWidget();

    /// Implementation of setter method for checked state text.
    /// \param text - Text of checkbox in checked state
    void SetOnText(const QString& text);

    /// Implementation of setter method for un-checked state text.
    /// \param checkboxText - Text of checkbox in un-checked state
    void SetOffText(const QString& text);

    /// Update the indicator size to reflect the current font metrics.
    /// If the font is changed programmatically, this should be called immediately after.
    void UpdateIndicatorSize();

    /// Setter for label_text property.
    /// \param text - The new value for the text label.
    void SetLabelText(const QString& text);

    /// Override setText
    /// This ensures that the text of the button is properly updated, even if the
    /// developer enters default text in the .ui file.
    virtual void setText(const QString& text);

protected:
    /// Override the sizeHint of the base QCheckBox.
    /// \return Properly scaled sizeHint.
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

private:
    QString on_text_;   ///< text of checkbox in checked state
    QString off_text_;  ///< text of checkbox in unchecked state

    /// Simple stylesheet template for updating the indicator size when the DPI scale changes.
    /// This ties the indicator size to the font size,
    /// and assumes a 2:1 width:height ratio of the indicator icons.
    const QString kIndicatorSizeStylesheet = "CheckBoxWidget::indicator { width: %1px; height: %2px; }";

private slots:
    /// Handler for ScaleFactorChanged signals.
    /// Invalidates font sizes and resizes the checkbox indicator.
    void OnScaleFactorChanged();

    /// Implementation of state change for this item.
    /// \param checkbox_state The current state of checkbox.
    void StateChanged(int checkbox_state);
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_CHECK_BOX_WIDGET_H_
