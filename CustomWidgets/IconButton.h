//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header file for a QPushButton derived widget with vector based icons
/// for various states.
//=============================================================================

#ifndef _ICON_BUTTON_H_
#define _ICON_BUTTON_H_

#include <QPushButton>
#include <QIcon>
#include <QString>

class QPaintEvent;

/// This class implements an Icon button
class IconButton : public QPushButton
{
    Q_OBJECT

public:
    IconButton(QWidget* pParent, const QString& normalIconResource, const QString& hoverIconResource, const QString& pressedIconResource, const QString& disabledIconResource);
    virtual ~IconButton() {}

    void SetNormalIcon(const QIcon& icon);
    void SetHoverIcon(const QIcon& icon);
    void SetPressedIcon(const QIcon& icon);
    void SetDisabledIcon(const QIcon& icon);


protected:
    virtual void paintEvent(QPaintEvent* pEvent);
    virtual void enterEvent(QEvent* event);
    virtual void leaveEvent(QEvent* pEvent);

private:
    QIcon m_normalIcon;     ///< The button's default icon
    QIcon m_hoverIcon;      ///< The icon used when mouse is over the button
    QIcon m_pressedIcon;    ///< The icon used when the button is pressed
    QIcon m_disabledIcon;   ///< The icon used when the button is disabled
};

#endif // _ICON_BUTTON_H_
