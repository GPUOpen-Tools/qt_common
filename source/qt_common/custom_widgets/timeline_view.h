//=============================================================================
/// Copyright (c) 2018-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header for a timeline view.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_TIMELINE_VIEW_H_
#define QTCOMMON_CUSTOM_WIDGETS_TIMELINE_VIEW_H_

#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "ruler_widget.h"

struct TimelineItem
{
    QGraphicsItem* item;   ///< Object to add to scene
    qint64         clock;  ///< What clock does it sit at
};

/// Holds and controls the timeline visualization
class TimelineView : public QGraphicsView
{
    Q_OBJECT

public:
    /// Constructor
    /// \param parent The parent widget.
    explicit TimelineView(QWidget* parent);

    /// Destructor which cleans up some of the graphics items in the timeline.
    virtual ~TimelineView();

    /// Handle resizing
    /// If reimplementing this in a child class, be sure to also call this base class' function.
    virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

    /// Callback for a mouse move event.
    /// If reimplementing this in a child class, be sure to also call this base class' function.
    /// \param event The QMouseEvent.
    virtual void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

    /// Callback for a mouse release event.
    /// If reimplementing this in a child class, be sure to also call this base class' function.
    /// \param event The QMouseEvent
    virtual void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

    /// Callback for a mouse press event.
    /// If reimplementing this in a child class, be sure to also call this base class' function.
    /// \param event The QMouseEvent
    virtual void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

    /// Update objects inside the timeline
    /// This gets called automatically by UpdateScene()
    /// Should be implemented by child classes in order to update x and y positions in the scene
    /// If reimplementing this in a child class, be sure to also call this base class' function.
    /// Top-left corner is (0,0) and bottom-right corner is (viewWidth,viewHeight)
    virtual void UpdateContent();

    /// Zoom in within current selection.
    /// \return true if further zooming in is possible, false otherwise
    bool ZoomInSelection();

    /// Reset the view back to default.
    void ZoomReset();

    /// Zoom in.
    /// \param zoom_rate The zoom rate (a value of 2 would be a x2 zoom in).
    /// \param use_mouse_position If true, zoom around the mouse position, otherwise zoom
    ///  around the view center position.
    /// \return true if further zooming in is possible, false otherwise
    bool ZoomIn(int zoom_rate = 2, bool use_mouse_position = false);

    /// Zoom out.
    /// \param zoom_rate The zoom rate (a value of 2 would be a x2 zoom out).
    /// \param use_mouse_position If true, zoom around the mouse position, otherwise zoom
    ///  around the view center position.
    /// \return true if further zooming out is possible, false otherwise
    bool ZoomOut(int zoom_rate = 2, bool use_mouse_position = false);

    /// Zoom in at x1 - x2 rate at last mouse move position
    /// \return true if further zooming in is possible, false otherwise
    bool ZoomInMousePosition();

    /// Zoom out at x1 - x2 rate at last mouse move position
    /// \return true if further zooming in is possible, false otherwise
    bool ZoomOutMousePosition();

    /// Hide timeline ruler
    void HideRuler();

    /// Show timeline ruler
    void ShowRuler();

    /// Get the viewable start clock
    /// \return viewable start clock
    qint64 ViewableStartClk() const;

    /// Get the viewable end clock
    /// \return viewable end clock
    qint64 ViewableEndClk() const;

    /// Get the selected start clock
    /// \return selected start clock
    qint64 SelectedStartClk() const;

    /// Get the selected end clock
    /// \return selected end clock
    qint64 SelectedEndClk() const;

    /// Get the hovered-over clock
    /// \return hovered-over clock
    qint64 HoverClk() const;

    /// Get the left scene bound value
    /// \return left scene bound
    int LeftSceneBound() const;

    /// Get the right scene bound value
    /// \return right scene bound
    int RightSceneBound() const;

    /// Get whether a selection has been made
    /// \return true if a selection has been made; false otherwise.
    bool RegionSelected() const;

    /// Convert a clock cycle to the scene's x-position
    /// Can be used to position scene items.
    /// \param clock The input clock
    int ClockToSceneCoordinate(qint64 clock) const;

    /// Convert a scene's x-position to a clock cycle
    /// \param sceneCoordX The scene x-position
    qint64 SceneCoordinateToClock(int sceneCoordX) const;

    /// Set ruler max clock
    /// \param clock new max clock
    void SetMaxClock(qint64 clock);

    /// Report back whether the widget is in reset state
    /// \return the bool backing reset state
    bool GetResetState() const;

protected:
    /// Update everything in the scene.
    void UpdateScene();

    /// Update vertical mouse indicator.
    void UpdateMouseIndicator();

    /// Redraw the selection box.
    void UpdateSelectionBox();

    /// Update ruler state.
    void UpdateRuler();

    /// Update scene bounding rect and limits.
    void UpdateSceneBounds();

    /// Gets the minimum ruler width.
    /// \return The minimum width of the ruler.
    int MinimumRulerWidth() const;

    /// Return the position of the bottom-most pixel right above the scrollbar.
    /// \return bottom pos.
    int BasePosY() const;

    /// Zoom in to a custom clock range.
    /// \param begin_clk The starting clock.
    /// \param end_clk The ending clock.
    /// \return true if further zooming in is possible, false otherwise.
    bool ZoomInClk(qint64 begin_clk, qint64 end_clk);

    /// Zoom out to a custom clock range.
    /// \param begin_clk The starting clock.
    /// \param end_clk The ending clock.
    /// \param zoom_rate The zoom rate (a value of 2 would be a x2 zoom out).
    /// \return true if further zooming out is possible, false otherwise.
    bool ZoomOutClk(qint64 begin_clk, qint64 end_clk, int zoom_rate);

    /// Zoom in at x1 - x2 rate at last mouse move position.
    /// \param last_hovered_clk The time at the last mouse move position.
    /// \param zoom_rate The zoom rate (a value of 2 would be a x2 zoom in).
    /// \return true if further zooming in is possible, false otherwise.
    bool ZoomInMousePositionClk(qint64 last_hovered_clk, int zoom_rate);

    /// Zoom out at x1 - x2 rate at last mouse move position.
    /// \param last_hovered_clk The time at the last mouse move position.
    /// \param zoom_rate The zoom rate (a value of 2 would be a x2 zoom out).
    /// \return true if further zooming in is possible, false otherwise.
    bool ZoomOutMousePositionClk(qint64 last_hovered_clk, int zoom_rate);

    /// Clamp input clocks to within ruler range. Take into account minimum zoom level.
    /// \param begin_clk The starting clock.
    /// \param end_clk The ending clock.
    void ClampClocks(qint64& begin_clk, qint64& end_clk);

    /// Move the scrollbar to the right position after zoom effect.
    /// \param begin_clk The left-most visible.
    void SetScrollbarAtClock(qint64 begin_clk);

    /// Reset the UI back to starting state.
    void SetResetState();

    RulerConfig ruler_config_;  ///< Ruler config

    RulerWidget*       ruler_;            ///< Ruler widget
    QGraphicsLineItem* mouse_indicator_;  ///< Vertical mouse indicator widget
    QGraphicsRectItem* selection_box_;    ///< Selection box widget
    QGraphicsScene*    scene_;            ///< Scene containing all widgets

    bool   is_selection_defined_;  ///< Track whether the selection changed
    bool   is_selection_held_;     ///< Track as the user makes a selection
    int    selection_pos_x_;       ///< Track the starting position for selection
    qint64 selected_start_clock_;  ///< The visible start clock
    qint64 selected_end_clock_;    ///< The visible end clock

    qint64 viewable_start_clock_;  ///< The visible start clock
    qint64 viewable_end_clock_;    ///< The visible end clock

    qint64 last_hovered_clock_;  ///< Remember where we last moved the mouse over

    double ruler_zoom_factor_;  ///< Track how much we've zoomed in

    int left_scene_bound_;   ///< Scene x-coordinate of the visible left edge
    int right_scene_bound_;  ///< Scene x-coordinate of the visible right edge

    bool in_reset_state_;  ///< Timeline is in starting state

    bool is_ruler_hidden_;  ///< Flag for ruler visibility

    QVector<TimelineItem> content_;  ///< Vector of all objects in the scene added by child implementations

private slots:
    /// Track scrollbar changes
    void ScrollBarChanged();
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_TIMELINE_VIEW_H_
