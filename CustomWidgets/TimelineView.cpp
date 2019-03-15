//=============================================================================
/// Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a timeline
//=============================================================================

#include "TimelineView.h"

#include <QApplication>
#include <QStyle>
#include <QScrollBar>
#include <QDebug>
#include <QMouseEvent>
#include <QAbstractScrollArea>

#include <../Scaling/ScalingManager.h>
#include <../Util/CommonDefinitions.h>

static const QColor s_SELECTION_COLOR = QColor(0, 118, 215);
static const quint64 s_ZOOM_TO_SELECTION_PERCENTAGE = 10;

//-----------------------------------------------------------------------------
/// Constructor
/// \param sceneCfg Configuration struct
//-----------------------------------------------------------------------------
TimelineView::TimelineView(QWidget* pParent) :
    QGraphicsView(pParent),
    m_pRuler(nullptr),
    m_pMouseIndicator(nullptr),
    m_pSelectionBox(nullptr),
    m_pScene(nullptr),
    m_selectionDefined(false),
    m_selectionHeld(false),
    m_selectionPosX(0),
    m_selectedStartClk(0),
    m_selectedEndClk(0),
    m_viewableStartClk(0),
    m_viewableEndClk(0),
    m_lastHoveredClock(0),
    m_rulerZoomFactor(1.0f),
    m_leftSceneBound(0),
    m_rightSceneBound(0),
    m_resetState(true)
{
    setMouseTracking(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    verticalScrollBar()->blockSignals(true);

    m_pScene = new QGraphicsScene();
    setScene(m_pScene);

    m_rulerCfg = {};
    m_rulerCfg.maxTime = 10000;
    m_rulerCfg.height = s_RULER_HEIGHT;
    m_rulerCfg.width = this->width();
    m_rulerCfg.scaleFactor = ScalingManager::Get().GetScaleFactor();
    m_rulerCfg.timeToClockRatio = 1;
    m_rulerCfg.unitType = TIME_UNIT_TYPE_CLK;

    m_pRuler = new RulerWidget(m_rulerCfg);
    m_pRuler->setPos(0, 0);

    const int regionHeight = (height() - ScaledRulerHeight()) / 2;
    const int selectionPosY = regionHeight - (ScaledRulerHeight() / 2) - 2;

    m_pMouseIndicator = new QGraphicsRectItem();
    m_pMouseIndicator->setRect(0, s_RULER_HEIGHT * ScalingManager::Get().GetScaleFactor(), 1 * ScalingManager::Get().GetScaleFactor(), height() - ScaledRulerHeight());
    m_pMouseIndicator->setBrush(Qt::gray);
    m_pMouseIndicator->setZValue(1);
    m_pMouseIndicator->setPen(Qt::NoPen);

    m_pSelectionBox = new QGraphicsRectItem(QRectF(0, selectionPosY*-1, 0, height() - ScaledRulerHeight()));
    m_pMouseIndicator->setRect(0, s_RULER_HEIGHT * ScalingManager::Get().GetScaleFactor(), 1 * ScalingManager::Get().GetScaleFactor(), height() - ScaledRulerHeight());
    m_pSelectionBox->setBrush(s_SELECTION_COLOR);
    m_pSelectionBox->setPen(Qt::NoPen);
    m_pSelectionBox->setOpacity(0.0);
    m_pSelectionBox->setZValue(1);

    m_pScene->addItem(m_pRuler);
    m_pScene->addItem(m_pMouseIndicator);
    m_pScene->addItem(m_pSelectionBox);

    UpdateScene();

    connect(horizontalScrollBar(), &QScrollBar::valueChanged, this, &TimelineView::ScrollBarChanged);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TimelineView::~TimelineView()
{
    delete m_pMouseIndicator;
    delete m_pSelectionBox;
    delete m_pRuler;
}

//-----------------------------------------------------------------------------
/// Update scene bounding rect and limits
/// \param viewType the view type
//-----------------------------------------------------------------------------
void TimelineView::UpdateSceneBounds()
{
    // Update the scene rect/bounds because the ruler changed length
    //const QRect sceneRect = QRect((m_rulerCfg.width / 2)*-1, (height() / 2)*-1, m_rulerCfg.width, height());
    //m_pScene->setSceneRect(sceneRect);

    const QRectF sceneRect = QRectF(0, 0, m_rulerCfg.width, basePosY());
    m_pScene->setSceneRect(sceneRect);

    m_leftSceneBound = mapToScene(QPoint(0, 0)).x();
    m_rightSceneBound = mapToScene(QPoint(width(), 0)).x();

    if (m_resetState)
    {
        m_viewableStartClk = 0;
        m_viewableEndClk = m_rulerCfg.maxTime;
    }
    else
    {
        m_viewableStartClk = SceneCoordinateToClock(m_leftSceneBound);

        m_viewableEndClk = SceneCoordinateToClock(m_rightSceneBound);

        ClampClocks(m_viewableStartClk, m_viewableEndClk);
    }

    m_pRuler->UpdateVisibleBounds(m_viewableStartClk, m_viewableEndClk);
}

//-----------------------------------------------------------------------------
/// Convert a clock cycle to the scene's x-position
/// \param clock The input clock
//-----------------------------------------------------------------------------
int TimelineView::ClockToSceneCoordinate(qint64 clock)
{
    //return clock * ((double)m_rulerCfg.width / (double)m_rulerCfg.maxTime) - (m_rulerCfg.width / 2);
    return clock * ((double)m_rulerCfg.width / (double)m_rulerCfg.maxTime);// -(m_rulerCfg.width / 2);
}

//-----------------------------------------------------------------------------
/// Convert a scene's x-position to a clock cycle
/// \param sceneCoordX The scene x-position
//-----------------------------------------------------------------------------
qint64 TimelineView::SceneCoordinateToClock(int sceneCoordX)
{
    //return (sceneCoordX + m_rulerCfg.width / 2) * ((double)m_rulerCfg.maxTime / (double)m_rulerCfg.width);
    return (sceneCoordX) * ((double)m_rulerCfg.maxTime / (double)m_rulerCfg.width);
}

//-----------------------------------------------------------------------------
/// Update ruler state
//-----------------------------------------------------------------------------
void TimelineView::UpdateRuler()
{
    m_rulerCfg.maxTime = ((qint64)m_rulerCfg.maxTime > width()) ? m_rulerCfg.maxTime : width();

    m_pRuler->Update(m_rulerCfg);

    // Scene bounds is driven by ruler width, so update
    UpdateSceneBounds();

    // Move ruler to top left
    //const QPointF pos = QPointF((m_rulerCfg.width / 2) * -1, (height() / 2) * -1 + m_rulerCfg.height / 2);
    //m_pRuler->setPos(pos);
}

//-----------------------------------------------------------------------------
/// Set ruler max clock
/// \param clock new max clock
//-----------------------------------------------------------------------------
void TimelineView::SetMaxClock(qint64 clock)
{
    m_rulerCfg.maxTime = clock;

    UpdateScene();
}

//-----------------------------------------------------------------------------
/// Report back whether we're in reset state
/// \return the bool backing reset state
//-----------------------------------------------------------------------------
bool TimelineView::GetResetState()
{
    return m_resetState;
}

//-----------------------------------------------------------------------------
/// Track scrollbar changes
//-----------------------------------------------------------------------------
void TimelineView::ScrollBarChanged()
{
    UpdateScene();
}

//-----------------------------------------------------------------------------
/// Update vertical mouse indicator
//-----------------------------------------------------------------------------
void TimelineView::UpdateMouseIndicator()
{
    QRectF rect = m_pMouseIndicator->rect();

    if (m_rulerHidden == true)
    {
        rect.setY(0);
    }
    else
    {
        rect.setY((s_RULER_HEIGHT + 1) * ScalingManager::Get().GetScaleFactor());
    }

    rect.setHeight(height());

    m_pMouseIndicator->setRect(rect);
    m_pMouseIndicator->setX(ClockToSceneCoordinate(m_lastHoveredClock));
}

//-----------------------------------------------------------------------------
/// Redraw the selection box
//-----------------------------------------------------------------------------
void TimelineView::UpdateSelectionBox()
{
    const int sceneStartPosX = ClockToSceneCoordinate(m_selectedStartClk);
    const int sceneEndPosX = ClockToSceneCoordinate(m_selectedEndClk);

    QRectF rect = m_pSelectionBox->rect();
    rect.setX(sceneStartPosX);

    if (m_rulerHidden == true)
    {
        rect.setY(0);
    }
    else
    {
        rect.setY((s_RULER_HEIGHT + 1) * ScalingManager::Get().GetScaleFactor());
    }

    rect.setWidth(sceneEndPosX - sceneStartPosX);
    rect.setHeight(height());
    m_pSelectionBox->setRect(rect);

    if (m_selectionDefined == true)
    {
        m_pSelectionBox->setOpacity(0.5);
    }
}

//-----------------------------------------------------------------------------
/// Update objects inside the timeline
/// This gets called automatically by UpdateScene()
/// Child classes should implement this to update x and y positions
/// Top-left corner is (0,0) and bottom-right corner is (viewWidth,viewHeight)
//-----------------------------------------------------------------------------
void TimelineView::UpdateContent()
{
}

//-----------------------------------------------------------------------------
/// Update everything in the scene
//-----------------------------------------------------------------------------
void TimelineView::UpdateScene()
{
    UpdateRuler();
    UpdateMouseIndicator();
    UpdateSelectionBox();
    UpdateContent();

    viewport()->update();
}

//-----------------------------------------------------------------------------
/// Detect a mouse press event.
/// \param dragEnabled is dragging active
//-----------------------------------------------------------------------------
void TimelineView::mousePressEvent(QMouseEvent* pEvent)
{
    QGraphicsView::mousePressEvent(pEvent);

    if (pEvent->button() != Qt::RightButton)
    {
        const QPoint mouseHoverCoords = mapFromGlobal(QCursor::pos());
        const QPointF sceneHoverCoords = mapToScene(mouseHoverCoords);

        m_selectionPosX = sceneHoverCoords.x();

        m_selectionDefined = false;

        m_pSelectionBox->setOpacity(0);

        m_selectionHeld = true;

        m_selectedStartClk = 0;
        m_selectedEndClk = 0;
    }
}

//-----------------------------------------------------------------------------
/// Detect a mouse release event.
//-----------------------------------------------------------------------------
void TimelineView::mouseReleaseEvent(QMouseEvent* pEvent)
{
    QGraphicsView::mouseReleaseEvent(pEvent);

    if (pEvent->button() != Qt::RightButton)
    {
        m_selectionHeld = false;
    }
}

//-----------------------------------------------------------------------------
/// Capture a mouse move event.
/// \param pEvent The move event.
//-----------------------------------------------------------------------------
void TimelineView::mouseMoveEvent(QMouseEvent* pEvent)
{
    QGraphicsView::mouseMoveEvent(pEvent);

    const QPoint mouseHoverCoords = mapFromGlobal(QCursor::pos());
    const QPointF sceneHoverCoords = mapToScene(mouseHoverCoords);

    qint64 potentialHoveredClock = SceneCoordinateToClock(sceneHoverCoords.x());

    if ((potentialHoveredClock >= 0) && (potentialHoveredClock <= (qint64)m_rulerCfg.maxTime))
    {
        m_lastHoveredClock = potentialHoveredClock;
    }

    if (pEvent->button() != Qt::RightButton)
    {
        if (m_selectionHeld == true)
        {
            QRectF rect = m_pSelectionBox->rect();

            const int futureDelta = sceneHoverCoords.x() - m_selectionPosX;

            if (futureDelta >= 0)
            {
                rect.setX(m_selectionPosX);
                rect.setWidth(futureDelta);
            }
            else
            {
                rect.setX(m_selectionPosX + futureDelta);
                rect.setWidth(futureDelta*-1);
            }

            m_pSelectionBox->setRect(rect);

            m_pSelectionBox->setOpacity(0.5);

            m_selectionDefined = true;

            m_selectedStartClk = SceneCoordinateToClock(rect.x());

            m_selectedEndClk = SceneCoordinateToClock(rect.x() + rect.width());

            ClampClocks(m_selectedStartClk, m_selectedEndClk);
        }
    }

    UpdateMouseIndicator();

    // May enable for debugging
#if 0
    QPoint mouseCoords = mapFromGlobal(QCursor::pos());
    QPointF sceneCoords = mapToScene(mouseCoords);
    qDebug() << "mouse: " << mouseCoords;
    qDebug() << "sceneCoords: " << sceneCoords;
    qDebug() << "---------";
#endif
}

//-----------------------------------------------------------------------------
/// Handle resizing
//-----------------------------------------------------------------------------
void TimelineView::resizeEvent(QResizeEvent* pEvent)
{
    QGraphicsView::resizeEvent(pEvent);

    if (m_resetState == true)
    {
        m_rulerCfg.width = MinimumRulerWidth();
    }
    else
    {
        if (m_pRuler->boundingRect().width() < width())
        {
            SetResetState();
        }
    }

    UpdateScene();
}

//-----------------------------------------------------------------------------
/// Zoom in at x1 - x2 rate at last mouse move position
/// \return true if further zooming in is possible, false otherwise
//-----------------------------------------------------------------------------
bool TimelineView::ZoomInMousePosition()
{
    return ZoomInMousePositionClk(m_lastHoveredClock, 2);
}

//-----------------------------------------------------------------------------
/// Zoom in within current selection.
/// \return true if further zooming in is possible, false otherwise
//-----------------------------------------------------------------------------
bool TimelineView::ZoomInSelection()
{
    if (m_selectionDefined == true)
    {
        quint64 midpoint = (m_selectedStartClk + m_selectedEndClk) / 2;
        quint64 range = ((m_selectedEndClk - m_selectedStartClk) * (s_ZOOM_TO_SELECTION_PERCENTAGE + 100)) / 100;
        quint64 halfRange = range / 2;

        return ZoomInClk(midpoint - halfRange, midpoint + halfRange);
    }

    return false;
}

//-----------------------------------------------------------------------------
/// Zoom in at x1 - x2 rate at last mouse move position
/// \param lastHoveredClock The time at the last mouse move position
/// \param zoomRate The zoom rate (a value of 2 would be a x2 zoom in).
/// \return true if further zooming in is possible, false otherwise
//-----------------------------------------------------------------------------
bool TimelineView::ZoomInMousePositionClk(qint64 lastHoveredClock, int zoomRate)
{
    const quint64 leftRatio = (lastHoveredClock - m_viewableStartClk) / zoomRate;
    const quint64 rightRatio = (m_viewableEndClk - lastHoveredClock) / zoomRate;

    const qint64 beginClk = lastHoveredClock - leftRatio;
    const qint64 endClk = lastHoveredClock + rightRatio;

    return ZoomInClk(beginClk, endClk);
}

//-----------------------------------------------------------------------------
/// Zoom in.
/// \param zoomRate The zoom rate (a value of 2 would be a x2 zoom in).
/// \param useMousePos If true, zoom around the mouse position, otherwise zoom
///  around the view center position.
/// \return true if further zooming in is possible, false otherwise
//-----------------------------------------------------------------------------
bool TimelineView::ZoomIn(int zoomRate, bool useMousePos)
{
    if (useMousePos == false)
    {
        const qint64 desiredClockRange = (m_viewableEndClk - m_viewableStartClk) / zoomRate;
        const qint64 desiredClockMidPoint = (m_viewableEndClk + m_viewableStartClk) / 2;
        const qint64 desiredLeftClock = desiredClockMidPoint - (desiredClockRange / 2);
        const qint64 desiredRightClock = desiredLeftClock + desiredClockRange;

        return ZoomInClk(desiredLeftClock, desiredRightClock);
    }
    else
    {
        return ZoomInMousePositionClk(m_lastHoveredClock, zoomRate);
    }
}

//-----------------------------------------------------------------------------
/// Move the scrollbar to the right position after zoom effect.
/// \param clk The left-most visible
//-----------------------------------------------------------------------------
void TimelineView::SetScrollbarAtClock(qint64 beginClk)
{
    QScrollBar* pGraphicsScroll = horizontalScrollBar();

    const double clockPct = (double)beginClk / (double)m_rulerCfg.maxTime;
    const int scrollBarOffset = pGraphicsScroll->pageStep() * clockPct * m_rulerZoomFactor;
    const int scollBarNewVal = pGraphicsScroll->minimum() + scrollBarOffset;

    pGraphicsScroll->setValue(scollBarNewVal);
}

//-----------------------------------------------------------------------------
/// Zoom into a custom clock range.
/// \param beginClock The starting clock
/// \param endClk The ending clock
/// \return true if further zooming in is possible, false otherwise
//-----------------------------------------------------------------------------
bool TimelineView::ZoomInClk(qint64 beginClk, qint64 endClk)
{
    // Reset the view before zooming to preserve accuracy.
    m_rulerCfg.width = MinimumRulerWidth();
    m_rulerZoomFactor = 1.0f;
    UpdateScene();

    qint64 selectedClockRange = endClk - beginClk;

    // if the selected range is too small, zoom in as far as possible
    if (selectedClockRange < width())
    {
        qint64 halfWidth = (width() - selectedClockRange) / 2;
        beginClk -= halfWidth;
        endClk += halfWidth;
    }

    bool canZoomFurther = false;

    // Clamp to within range
    ClampClocks(beginClk, endClk);

    // Recompute the range in case of clamps
    selectedClockRange = endClk - beginClk;

    // Compute a factor to scale the ruler by
    const qint64 visibleClockRange = m_viewableEndClk - m_viewableStartClk;
    const double zoomFactor = (double)visibleClockRange / (double)selectedClockRange;

    m_rulerZoomFactor *= zoomFactor;

    // Scale the ruler and update everything
    m_rulerCfg.width *= zoomFactor;

    m_resetState = false;

    UpdateScene();

    SetScrollbarAtClock(beginClk);

    // check if further zooming is possible
    qint64 scr = (m_viewableEndClk - m_viewableStartClk) / 2;
    if (scr > width())
    {
        canZoomFurther = true;
    }

    return canZoomFurther;
}

//-----------------------------------------------------------------------------
/// Zoom out at x1 - x2 rate at last mouse move position
/// \return true if further zooming in is possible, false otherwise
//-----------------------------------------------------------------------------
bool TimelineView::ZoomOutMousePosition()
{
    return ZoomOutMousePositionClk(m_lastHoveredClock, 2);
}

//-----------------------------------------------------------------------------
/// Zoom out at x1 - x2 rate at last mouse move position
/// \param lastHoveredClock The time at the last mouse move position
/// \param zoomRate The zoom rate (a value of 2 would be a x2 zoom out).
/// \return true if further zooming in is possible, false otherwise
//-----------------------------------------------------------------------------
bool TimelineView::ZoomOutMousePositionClk(qint64 lastHoveredClock, int zoomRate)
{
    const quint64 leftRatio = lastHoveredClock - m_viewableStartClk;
    const quint64 rightRatio = m_viewableEndClk - lastHoveredClock;

    const qint64 beginClk = lastHoveredClock - (leftRatio * zoomRate);
    const qint64 endClk = lastHoveredClock + (rightRatio * zoomRate);

    return ZoomOutClk(beginClk, endClk, zoomRate);
}

//-----------------------------------------------------------------------------
/// Hide timeline ruler
//-----------------------------------------------------------------------------
void TimelineView::HideRuler()
{
    if (scene() != nullptr)
    {
        // Check for item in scene before attempting
        // to remove
        if (scene()->items().contains(m_pRuler))
        {
            scene()->removeItem(m_pRuler);
            m_rulerHidden = true;
        }
    }
}

//-----------------------------------------------------------------------------
/// Show timeline ruler
//-----------------------------------------------------------------------------
void TimelineView::ShowRuler()
{
    if (scene() != nullptr)
    {
        // Check for item already existing, if so ignore
        // request
        if (!scene()->items().contains(m_pRuler))
        {
            scene()->addItem(m_pRuler);
            m_rulerHidden = false;
        }
    }
}

//-----------------------------------------------------------------------------
/// Reset the UI back to starting state
//-----------------------------------------------------------------------------
void TimelineView::SetResetState()
{
    m_rulerCfg.width = MinimumRulerWidth();
    m_rulerZoomFactor = 1.0f;
    m_resetState = true;
    m_selectionDefined = false;
    m_selectedStartClk = 0;
    m_selectedEndClk = 0;
}

//-----------------------------------------------------------------------------
/// Clamp input clocks to within ruler range. Take into account minimum zoom
/// level.
/// \param beginClk The starting clock
/// \param endClk The ending clock
//-----------------------------------------------------------------------------
void TimelineView::ClampClocks(qint64& beginClk, qint64& endClk)
{
    if (beginClk < 0)
    {
        beginClk = 0;

        if (endClk < width())
        {
            endClk = width();
        }
    }
    else if (endClk > (qint64)m_rulerCfg.maxTime)
    {
        endClk = m_rulerCfg.maxTime;

        if ((endClk - beginClk) < width())
        {
            beginClk = endClk - width();
        }
    }
}

//-----------------------------------------------------------------------------
/// Zoom out.
/// \param zoomRate The zoom rate (a value of 2 would be a x2 zoom out).
/// \param useMousePos If true, zoom around the mouse position, otherwise zoom
///  around the view center position.
/// \return true if further zooming out is possible, false otherwise
//-----------------------------------------------------------------------------
bool TimelineView::ZoomOut(int zoomRate, bool useMousePos)
{
    if (useMousePos == false)
    {
        const qint64 selectedClockRange = m_viewableEndClk - m_viewableStartClk;
        const qint64 offset = (selectedClockRange * (zoomRate - 1)) / 2;
        qint64 beginClk = m_viewableStartClk - offset;
        qint64 endClk = m_viewableEndClk + offset;

        return ZoomOutClk(beginClk, endClk, zoomRate);
    }
    else
    {
        return ZoomOutMousePositionClk(m_lastHoveredClock, zoomRate);
    }
}

//-----------------------------------------------------------------------------
/// Zoom out.
/// \param beginClk The starting clock
/// \param endClk The ending clock
/// \param zoomRate The zoom rate (a value of 2 would be a x2 zoom out).
/// \return true if further zooming out is possible, false otherwise
//-----------------------------------------------------------------------------
bool TimelineView::ZoomOutClk(qint64 beginClk, qint64 endClk, int zoomRate)
{
    m_resetState = false;

    int potentialWidth = m_rulerCfg.width / zoomRate;

    if (potentialWidth > MinimumRulerWidth())
    {
        m_rulerZoomFactor /= zoomRate;

        // Clamp to within range
        ClampClocks(beginClk, endClk);

        m_rulerCfg.width = potentialWidth;

        UpdateScene();

        SetScrollbarAtClock(beginClk);

        return true;
    }
    else
    {
        SetResetState();

        UpdateScene();

        return false;
    }
}

//-----------------------------------------------------------------------------
/// Reset the view back to default.
//-----------------------------------------------------------------------------
void TimelineView::ZoomReset()
{
    SetResetState();

    UpdateScene();
}

//-----------------------------------------------------------------------------
/// Get the viewable start clock
/// \return viewable start clock
//-----------------------------------------------------------------------------
qint64 TimelineView::ViewableStartClk()
{
    return m_viewableStartClk;
}

//-----------------------------------------------------------------------------
/// Get the viewable end clock
/// \return viewable end clock
//-----------------------------------------------------------------------------
qint64 TimelineView::ViewableEndClk()
{
    return m_viewableEndClk;
}

//-----------------------------------------------------------------------------
/// Get the selected start clock
/// \return selected start clock
//-----------------------------------------------------------------------------
qint64 TimelineView::SelectedStartClk()
{
    return m_selectedStartClk;
}

//-----------------------------------------------------------------------------
/// Get the selected end clock
/// \return selected end clock
//-----------------------------------------------------------------------------
qint64 TimelineView::SelectedEndClk()
{
    return m_selectedEndClk;
}

//-----------------------------------------------------------------------------
/// Get the hovered-over clock
/// \return hovered-over clock
//-----------------------------------------------------------------------------
qint64 TimelineView::HoverClk()
{
    return m_lastHoveredClock;
}

//-----------------------------------------------------------------------------
/// Get the whether a selection has been made
/// \return whether a selection has been made
//-----------------------------------------------------------------------------
bool TimelineView::RegionSelected()
{
    return m_selectionDefined;
}

//-----------------------------------------------------------------------------
/// Return scaled ruler height
/// \return scaled height
//-----------------------------------------------------------------------------
int TimelineView::ScaledRulerHeight()
{
    return m_rulerCfg.height * ScalingManager::Get().GetScaleFactor();
}

//-----------------------------------------------------------------------------
/// Return minimum ruler width
/// \return ruler width
//-----------------------------------------------------------------------------
int TimelineView::MinimumRulerWidth()
{
    return (width() - 2);
}

//-----------------------------------------------------------------------------
/// Return the position of the bottom-most pixel right above the scrollbar
/// \return bottom pos
//-----------------------------------------------------------------------------
int TimelineView::basePosY()
{
    if (horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOn || horizontalScrollBarPolicy() == Qt::ScrollBarAsNeeded)
    {
        return height() - qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent) - 3 * ScalingManager::Get().GetScaleFactor();
    }
    else
    {
        return height();
    }
}
