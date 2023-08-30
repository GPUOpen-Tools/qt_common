//=============================================================================
/// Copyright (c) 2017-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header file for a QPushButton derived widget with vector based icons
/// for various states.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_ICON_BUTTON_H_
#define QTCOMMON_CUSTOM_WIDGETS_ICON_BUTTON_H_

#include <QPushButton>
#include <QIcon>
#include <QString>

class QPaintEvent;

/// This class implements an Icon button
class IconButton : public QPushButton
{
    Q_OBJECT

public:
    /// Standard constructor for the icon button.
    /// \param parent Pointer to the parent widget.
    IconButton(QWidget* parent = nullptr);

    /// Special constructor which sets icons for normal, hover, pressed, and disabled states.
    /// \param parent Pointer to the parent widget.
    /// \param normal_icon_resource The default icon resource string for the button.
    /// \param hover_icon_resource The resource string of the icon used when the mouse
    /// is over the button.
    /// \param pressed_icon_resource resource string of the icon used when the button is pressed.
    /// \param disabled_icon_resource resource string of the icon used when the button is disabled.
    IconButton(QWidget*       parent,
               const QString& normal_icon_resource,
               const QString& hover_icon_resource,
               const QString& pressed_icon_resource,
               const QString& disabled_icon_resource);

    /// Destructor
    virtual ~IconButton();

    /// Set the default icon for the button.
    /// \param The normal icon.
    void SetNormalIcon(const QIcon& icon);

    /// Set the icon used when the mouse hovers over the button.
    /// \param The hover icon.
    void SetHoverIcon(const QIcon& icon);

    /// Set the icon used when the button is pressed.
    /// \param The pressed icon.
    void SetPressedIcon(const QIcon& icon);

    /// Set the icon used when the button is disabled.
    /// \param The disabled icon.
    void SetDisabledIcon(const QIcon& icon);

    /// Overridden sizeHint that returns a scaled size based on either a default
    /// size, or the smallest available size from the icon asset.
    /// \return A DPI-aware sizeHint.
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
    /// Overriden paint event for this IconButton.
    /// The QPaintEvent::rect() is only for a subset of the widget that needs repainting
    /// so use the widget's rect() since this always paints the full widget.
    /// \param event Pointer to the QPaintEvent object.
    virtual void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

    /// Event triggered when the mouse is over the button.
    /// \param event Pointer to the event object.
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    virtual void enterEvent(QEvent* event) Q_DECL_OVERRIDE;
#else
    virtual void enterEvent(QEnterEvent *event) Q_DECL_OVERRIDE;
#endif

    /// Event triggered when the mouse is no longer over the button.
    /// \param event Pointer to the event object.
    virtual void leaveEvent(QEvent* event) Q_DECL_OVERRIDE;

private:
    /// The default width and height of the icon if it does not have its own sizes.
    const int kDefaultIconSize = 16;

    QIcon normal_icon_;    ///< The button's default icon
    QIcon hover_icon_;     ///< The icon used when mouse is over the button
    QIcon pressed_icon_;   ///< The icon used when the button is pressed
    QIcon disabled_icon_;  ///< The icon used when the button is disabled
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_ICON_BUTTON_H_
