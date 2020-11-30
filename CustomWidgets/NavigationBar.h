//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for the custom browser navigation tool bar.
//=============================================================================

#ifndef _NAVIGATION_BAR_H_
#define _NAVIGATION_BAR_H_

#include <QAbstractButton>
#include <QHBoxLayout>

#include "IconButton.h"

/// NavigationBar widget. Customized toolbar for browser navigation.
class NavigationBar : public QAbstractButton
{
    Q_OBJECT
public:
    // Implementations for Qt
    explicit NavigationBar(QWidget* pParent = nullptr);
    virtual ~NavigationBar() {}
    virtual void paintEvent(QPaintEvent* e) { QWidget::paintEvent(e); }
    IconButton& BackButton() { return m_browseBackButton; }
    IconButton& ForwardButton() { return m_browseForwardButton; }

public slots:
    void EnableBackButton(bool enable);
    void EnableForwardButton(bool enable);

protected:
    virtual void mouseMoveEvent(QMouseEvent* pEvent);

private:
    QHBoxLayout         m_layout;                     ///< The layout for the navigation bar.
    IconButton       m_browseBackButton;           ///< The browse back navigation button.
    IconButton       m_browseForwardButton;        ///< The browse forward navigation button.
};


#endif // _NAVIGATION_BAR_H_

