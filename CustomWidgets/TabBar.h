//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for the custom tab bar.
//=============================================================================

#ifndef _TAB_BAR_H_
#define _TAB_BAR_H_

#include <QTabBar>

class QWidget;

/// Support for the custom Tab Bar Widget
class TabBar : public QTabBar
{
    Q_OBJECT
public:
    // Implementations for Qt
    explicit TabBar(QWidget* pParent = nullptr);
    virtual ~TabBar() {}
    void setTabEnabled(int index, bool enable);
    void SetSpacerIndex(int index);
    int SpacerIndex() const;
    int CalcSpacerWidth() const;
    void SetTabTool(int index, QWidget* pWidget);

protected:
    virtual void mouseMoveEvent(QMouseEvent* pEvent);
    virtual QSize tabSizeHint(int index) const;
    QSize minimumTabSizeHint(int index) const;
    virtual void paintEvent(QPaintEvent* pEvent);

private:
    int m_spacerIndex;                   ///< Index of the hidden tab between the left justified tabs and the right justified tabs
    int m_mouseHoverLastTabIndex;        ///< The index of the tab that the mouse was hovering over when the last mouseMoveEvent was processed.
};

#endif // _TAB_BAR_H_

