//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header file for a quick links QPushButton
//=============================================================================

#ifndef _QUICK_LINK_BUTTON_WIDGET_H_
#define _QUICK_LINK_BUTTON_WIDGET_H_

#include <QPushButton>

/// This class implements a rotated QPushButton
class QuickLinkButtonWidget : public QPushButton
{
    Q_OBJECT

public:
    explicit QuickLinkButtonWidget(QWidget* pParent = nullptr);
    virtual ~QuickLinkButtonWidget() {}

    void SetTitle(const QString& title) { m_titleString = title; }
    void SetDescLineOne(const QString& desc) { m_descLineOne = desc; }
    void SetDescLineTwo(const QString& desc) { m_descLineTwo = desc; }
    void SetScaleFactor(double scaleFactor) { m_scaleFactor = scaleFactor; }

protected:
    virtual void paintEvent(QPaintEvent* pEvent);
    virtual void enterEvent(QEvent* pEvent);
    virtual void leaveEvent(QEvent* pEvent);

private:
    QString m_titleString;   ///< Title string
    QString m_descLineOne;   ///< Description string one
    QString m_descLineTwo;   ///< Description string two

    double m_scaleFactor;    ///< Current scale factor
    QColor m_textColor;      ///< Current text color
};

#endif // _QUICK_LINK_BUTTON_WIDGET_H_
