//=============================================================================
/// Copyright (c) 2017-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header file for a QPushButton with three lines of text. The first
/// line is a title which is drawn in bold, and the second two lines are
/// description, drawn using the same font but not in bold. The text changes
/// color when the mouse is over the button.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_QUICK_LINK_BUTTON_WIDGET_H_
#define QTCOMMON_CUSTOM_WIDGETS_QUICK_LINK_BUTTON_WIDGET_H_

#include <QPushButton>

/// This class implements a rotated QPushButton
class QuickLinkButtonWidget : public QPushButton
{
    Q_OBJECT

public:
    /// Explicit constructor.
    /// \param parent The parent widget
    explicit QuickLinkButtonWidget(QWidget* parent = nullptr);

    /// Virtual destructor.
    virtual ~QuickLinkButtonWidget();

    /// Set the text for the title.
    /// \param desc The text for the title.
    void SetTitle(const QString& title)
    {
        title_string_ = title;
    }

    /// Set the text for the first line of the description.
    /// \param desc The text for the first line of the description.
    void SetDescLineOne(const QString& desc)
    {
        description_line_one_ = desc;
    }

    /// Set the text for the second line of the description.
    /// \param desc The text for the second line of the description.
    void SetDescLineTwo(const QString& desc)
    {
        description_line_two_ = desc;
    }

protected:
    /// Overridden paint event for this QPushButton.
    /// \param event The paint event
    virtual void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

    /// Overridden mouse hover enter event.
    /// \param event The event
    virtual void enterEvent(QEvent* event) Q_DECL_OVERRIDE;

    /// Overridden mouse hover leave event.
    /// \param event The event
    virtual void leaveEvent(QEvent* event) Q_DECL_OVERRIDE;

    /// Overridden sizeHint for the QPushButton
    /// Accounts for margins around the text, three lines of text, and line spacing between the text.
    /// \return The recommended size for this button.
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

private slots:
    void OnScaleFactorChanged();

private:
    const int    kLineSpacing_     = 5;                      ///< Number of pixels between lines of text
    const int    kContentsMargin_  = 5;                      ///< Number of pixels margin between text and edge of button
    const QColor kBackgroundColor_ = QColor(241, 241, 241);  ///< Background color of the button
    const QColor kHighlightColor_  = QColor(0, 0, 255);      ///< Color of text when the mouse is over the button

    QString title_string_;          ///< Title string
    QString description_line_one_;  ///< Description string one
    QString description_line_two_;  ///< Description string two
    QColor  text_color_;            ///< Current text color
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_QUICK_LINK_BUTTON_WIDGET_H_
