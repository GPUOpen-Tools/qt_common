//=============================================================================
/// Copyright (c) 2017-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for the custom tab bar.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_TAB_BAR_H_
#define QTCOMMON_CUSTOM_WIDGETS_TAB_BAR_H_

#include <QTabBar>

class QWidget;

/// Support for the custom Tab Bar Widget
class TabBar : public QTabBar
{
    Q_OBJECT

public:
    /// Explicit constructor
    /// \param parent The TabBar's parent widget.
    explicit TabBar(QWidget* parent = nullptr);

    /// Destructor
    virtual ~TabBar();

    /// Enable or disable tabs.
    ///
    /// Also resets the tab that the mouse was last detected to be hovering over.
    /// \param index The tab index to enable or disable.
    /// \param enable Set to true to enable or false to disable the tab.
    void setTabEnabled(int index, bool enable);

    /// Set the index of the tab which is used as a spacer between the left and
    /// right justified tabs.
    /// \param index The tab to be used as a spacer.
    void SetSpacerIndex(int index);

    /// Set if the last tab is used as a stretch to take up maximum space of parent widget.
    /// \param stretch True if last tab should stretch
    void SetStretchLast(bool stretch);

    /// Returns the index of the tab to be used as a spacer between the left justified
    /// and right justified tabs.
    /// \return The index of the spacer tab.  -1 indicates no spacer tab has been defined
    /// (i.e. all tabs on the tab bar are left justified).
    int SpacerIndex() const;

    /// Calculates the width of the spacer by subtracting the width of the left and right
    /// justified tabs from the total width of the tab bar.
    /// \return The width of the spacer tab.
    int CalcSpacerWidth() const;

    /// Replaces a tab with a widget (e.g. a toolbar button).
    /// \param index The index on the tab bar to be replaced.
    /// \Param button_widget A pointer to a button widget that will replace the tab
    /// (i.e. all tabs on the tab bar are left justified).
    /// \param pos The position of widget in tab.
    void SetTabTool(int index, QWidget* button_widget, QTabBar::ButtonPosition pos = QTabBar::LeftSide);

protected:
    /// Handle mouse move event.
    ///
    /// Change cursor depending on state of tab that the mouse is hovering over.
    /// \param event The mouseMoveEvent event.
    virtual void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

    /// Override for returning the tab size.
    ///
    /// If the size of the spacer tab is requested, the width is calculated to fill in
    /// the space between the left justified tabs and right justified tabs.
    /// \param index The tab to retrieve the size.
    /// \return A QSize object containing the width and height of the tab.
    virtual QSize tabSizeHint(int index) const Q_DECL_OVERRIDE;

    /// Override for returning the minimum tab size.
    ///
    /// If the size of the spacer tab is requested, return zero. This allows the tab widget
    /// to shrink if it is resized.
    /// \param index The tab to retrieve the size.
    /// \return A QSize object containing the width and height of the tab.
    QSize minimumTabSizeHint(int index) const Q_DECL_OVERRIDE;

private slots:
    void OnScaleFactorChanged();

private:
    bool stretch_last_;                ///< Flag to indicate tab bar should stretch the last tab
    int  spacer_index_;                ///< Index of the hidden tab between the left justified tabs and the right justified tabs
    int  mouse_hover_last_tab_index_;  ///< The index of the tab that the mouse was hovering over when the last mouseMoveEvent was processed.
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_TAB_BAR_H_
