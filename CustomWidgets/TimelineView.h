//=============================================================================
/// Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header for a timeline
//=============================================================================

#ifndef _TIMELINE_VIEW_H_
#define _TIMELINE_VIEW_H_

#include <QGraphicsView>

#include <../CustomWidgets/RulerWidget.h>

class QGraphicsScene;
class QGraphicsRectItem;
class QGraphicsItem;

struct TimelineItem
{
    QGraphicsItem* pItem;       ///< Object to add to scene
    qint64 clock;               ///< What clock does it sit at
};

/// Holds and controls the timeline visualization
class TimelineView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit TimelineView(QWidget* pParent);
    virtual ~TimelineView();

    // May be re-implemented by child class, but be sure to also call this parent class
    virtual void resizeEvent(QResizeEvent* pEvent);
    virtual void mouseMoveEvent(QMouseEvent* pEvent);
    virtual void mouseReleaseEvent(QMouseEvent* pEvent);
    virtual void mousePressEvent(QMouseEvent* pEvent);

    // Should be implemented by child class in order to update x and y positions in the scene
    virtual void UpdateContent();

    // To be called by the UI
    bool ZoomInSelection();
    void ZoomReset();
    bool ZoomIn(int zoomRate = 2, bool useMousePos = false);
    bool ZoomOut(int zoomRate = 2, bool useMousePos = false);
    bool ZoomInMousePosition();
    bool ZoomOutMousePosition();

    // Hide/Show ruler
    void HideRuler();
    void ShowRuler();

    // Get info
    qint64 ViewableStartClk();
    qint64 ViewableEndClk();
    qint64 SelectedStartClk();
    qint64 SelectedEndClk();
    qint64 HoverClk();
    bool RegionSelected();

    // Convert clocks to scene coords and vice versa. Can be used to position scene items.
    int ClockToSceneCoordinate(qint64 clock);
    qint64 SceneCoordinateToClock(int sceneCoordX);

    // Set max visible clock
    void SetMaxClock(qint64 clock);

	// Check reset state
    bool GetResetState();

protected:
    void UpdateScene();
    void UpdateMouseIndicator();
    void UpdateSelectionBox();
    void UpdateRuler();
    void UpdateSceneBounds();

    int ScaledRulerHeight();
    int MinimumRulerWidth();
    int basePosY();

    bool ZoomInClk(qint64 beginClk, qint64 endClk);
    bool ZoomOutClk(qint64 beginClk, qint64 endClk, int zoomRate);
    bool ZoomInMousePositionClk(qint64 lastHoveredClock, int zoomRate);
    bool ZoomOutMousePositionClk(qint64 lastHoveredClock, int zoomRate);

    void ClampClocks(qint64& beginClk, qint64& endClk);
    void SetScrollbarAtClock(qint64 beginClk);
    void SetResetState();

    RulerConfig m_rulerCfg;                     ///< Ruler config

    RulerWidget* m_pRuler;                      ///< Ruler widget
    QGraphicsRectItem* m_pMouseIndicator;       ///< Vertical mouse indicator widget
    QGraphicsRectItem* m_pSelectionBox;         ///< Selection box widget
    QGraphicsScene* m_pScene;                   ///< Scene containing all widgets

    bool m_selectionDefined;                    ///< Track whether the selection changed
    bool m_selectionHeld;                       ///< Track as the user makes a selection
    int m_selectionPosX;                        ///< Track the starting position for selection
    qint64 m_selectedStartClk;                  ///< The visible start clock
    qint64 m_selectedEndClk;                    ///< The visible end clock

    qint64 m_viewableStartClk;                  ///< The visible start clock
    qint64 m_viewableEndClk;                    ///< The visible end clock

    qint64 m_lastHoveredClock;                  ///< Remember where we last moved the mouse over

    double m_rulerZoomFactor;                   ///< Track how much we've zoomed in

    int m_leftSceneBound;                       ///< Scene x-coordinate of the visible left edge
    int m_rightSceneBound;                      ///< Scene x-coordinate of the visible right edge

    bool m_resetState;                          ///< Timeline is in starting state

    bool m_rulerHidden;                         ///< Flag for ruler visibility

    QVector<TimelineItem> m_content;            ///< Vector of all objects in the scene added by child implementations

private slots:
    void ScrollBarChanged();
};

#endif // _TIMELINE_VIEW_H_
