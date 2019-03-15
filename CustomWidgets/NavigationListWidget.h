//=============================================================================
/// Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header for a navigation list widget
//=============================================================================

#ifndef _NAVIGATION_LIST_WIDGET_H_
#define _NAVIGATION_LIST_WIDGET_H_

#include <QListWidget>

/// Wavefront graphics view that is aware of resize and mouse events
class NavigationListWidget : public QListWidget
{
    Q_OBJECT

public:
    NavigationListWidget(QWidget* pParent);
    virtual ~NavigationListWidget() {}

    void mouseMoveEvent(QMouseEvent* pEvent);
};

#endif // _NAVIGATION_LIST_WIDGET_H_
