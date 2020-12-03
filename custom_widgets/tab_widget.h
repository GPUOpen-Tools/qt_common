//=============================================================================
/// Copyright (c) 2017-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for the custom tab widget which adds support for right-
/// justified tabs.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_TAB_WIDGET_H_
#define QTCOMMON_CUSTOM_WIDGETS_TAB_WIDGET_H_

#include <QTabWidget>

#include "tab_bar.h"

/// Support for the custom Tab Bar Widget
class TabWidget : public QTabWidget
{
    Q_OBJECT

public:
    /// Explicit constructor
    /// \param parent The TabWidget's parent widget.
    explicit TabWidget(QWidget* parent = Q_NULLPTR);

    /// Destructor
    virtual ~TabWidget();

    /// Enable or disable tabs.
    /// \param index The tab index to enable or disable
    /// \param enable Set to true to enable or false to disable the tab.
    void setTabEnabled(int index, bool);

    /// Sets the index of the tab used as a spacer between left justified tabs and
    /// right justified tabs.
    /// \param index The index of the tab used as a spacer.
    void SetSpacerIndex(const int index);

    /// Sets the if the last tab should be used as stretch to take up
    /// maximum space of the parent widget.
    /// \param stretch True is last tab should stretch.
    void SetStretchLast(const bool stretch);

    /// Replaces a tab with a widget (e.g. a toolbar button).
    /// \param index The index on the tab bar to be replaced.
    /// \Param tool_widget A pointer to a button widget that will replace the tab.
    /// \param pos The position of the tool widget in tab.
    void SetTabTool(int index, QWidget* tool_widget, QTabBar::ButtonPosition pos = QTabBar::LeftSide);

    /// Returns the QTabBar's height.
    /// \return Height in pixels.
    int TabHeight() const;

protected:
    /// Returns the TabWidget's QTabBar object.
    /// \return A pointer to the tabBar associated with this TabWidget.
    QTabBar* tabBar() const;

    /// Handle a resize event.
    /// \param resize_event The resize event.
    virtual void resizeEvent(QResizeEvent* resize_event);

private:
    /// Custom tab bar.
    TabBar tab_bar_;
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_TAB_WIDGET_H_
