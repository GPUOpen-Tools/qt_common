//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for the custom tab widget.
//=============================================================================

#ifndef _TAB_WIDGET_H_
#define _TAB_WIDGET_H_

#include <QTabWidget>
#include "TabBar.h"

/// Support for the custom Tab Bar Widget
class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    // Implementations for Qt
    explicit TabWidget(QWidget *pParent = Q_NULLPTR);
    virtual ~TabWidget() {}
    void setTabEnabled(int index, bool);
    void SetSpacerIndex(const int index);
    void SetTabTool(int index, QWidget* pToolWidget);
    int TabHeight() const;

protected:
    QTabBar *tabBar() const;
    virtual void resizeEvent(QResizeEvent* pResizeEvent);

private:
    TabBar m_tabBar;         ///< Custom tab bar.
};

#endif // _TAB_WIDGET_H_

