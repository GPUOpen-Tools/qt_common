//=============================================================================
/// Copyright (c) 2016-2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation for a ruler.
//=============================================================================

#include <math.h>       /* log2 */
#include <QtWidgets>

#include <RulerWidget.h>
#include <../Scaling/ScalingManager.h>
#include <../Util/QtUtil.h>

// min and max tick step. This is the number of pixels between each tick or
// vertical line on the ruler. Currently assumes a value between 5 and 10 pixels
// is good.
static const double s_MIN_TICK_STEP = 5.0;
static const double s_MAX_TICK_STEP = 10.0;

static const QColor s_RULER_MARKER_COLOR = QColor(51, 51, 51);
static const QColor s_RULER_EDGE_COLOR = QColor(230, 230, 230);
static const QColor s_RULER_BACKGROUND_COLOR = QColor(248, 248, 248);
static const int s_RULER_PIXEL_FONT_SIZE = 11;

//-----------------------------------------------------------------------------
/// Static utility function to draw the ruler background. Allows external
/// implementations (rulered table headers) to draw the correct background
/// region.
/// \param pPainter Pointer to QPainter
/// \param rect The rect encompassing the whole ruler
//-----------------------------------------------------------------------------
void RulerWidget::PaintRulerBackground(QPainter* pPainter, const QRectF& rect)
{
    pPainter->setPen(s_RULER_EDGE_COLOR);
    pPainter->setBrush(s_RULER_BACKGROUND_COLOR);
    pPainter->drawRect(rect);
}

//-----------------------------------------------------------------------------
/// Get the start time of the ruler. A different value is needed for ruler
/// widgets attached to GraphicScene objects and QTreeView headers.
/// \param minVisibleTime The minimum time visible on the ruler
/// \param timePeriod The time period between each marker. Will depend on the
///  zoom level
/// \param useTimePeriod A flag indicating whether the time period should be
///  used in the calculation
//-----------------------------------------------------------------------------
double RulerWidget::GetStartingTime(double minVisibleTime, uint64_t timePeriod, bool useTimePeriod)
{
    if (useTimePeriod == false)
    {
        return minVisibleTime;
    }
    else
    {
        return (minVisibleTime - timePeriod) < 0 ? 0 : (minVisibleTime - timePeriod);
    }
}

//-----------------------------------------------------------------------------
/// Static utility function which holds ruler paint logic.
/// \param pPainter Pointer to QPainter
/// \param rect The rect encompassing the whole ruler
/// \param maxTime The maximum clock value
/// \param minVisibleClk The minimum visible clock value
/// \param maxVisibleClk The maximum visible clock value
/// \param scaleFactor Current scale factor
/// \param timeToClockRatio The time to clock ratio
/// \param unitType Working with clk, ns, us, or ms
//-----------------------------------------------------------------------------
void RulerWidget::PaintRuler(QPainter* pPainter, const QRectF& rect, uint64_t maxTime, uint64_t minVisibleClk, uint64_t maxVisibleClk, double scaleFactor, double timeToClockRatio, int unitType, bool useTimePeriod)
{
    pPainter->save();

    QFont font;
    font.setFamily(font.defaultFamily());
    font.setPixelSize(s_RULER_PIXEL_FONT_SIZE * scaleFactor);
    pPainter->setFont(font);

    QPen pen(s_RULER_MARKER_COLOR);
    pPainter->setPen(pen);

    // convert maxtime, minvisible, maxvisible to nanoseconds if time units not clock
    // This is so the ruler scaling can be done using the time units specified and
    // not just clocks.
    double maxClockTime = timeToClockRatio * (double)maxTime;
    double maxVisibleTime = timeToClockRatio * (double)maxVisibleClk;
    double minVisibleTime = timeToClockRatio * (double)minVisibleClk;

    // increment can go from 5 to 10
    const int32_t markersPerSection = 20;
    const int markerUnitHeight = rect.height() / 8;

    // time period is the time between the numeric values displayed on the ruler ie
    // if the ruler displayed 10000, 20000, 30000, the time period is 10000
    uint64_t timePeriod = 1;

    double rulerLength = rect.width();
    //RGP_ASSERT(rulerLength > 0);
    double numSections = (markersPerSection * maxClockTime);

    // calculate how big the tick step is for the whole ruler. This will be some
    // arbitrary value
    double tickStep = rulerLength / numSections;

    // If the ticks are too close together, keep multiplying by 10. This will force the ticks
    // further apart and will also increase the number displayed in each tick. For example, using
    // values of 0, 10, 20, 30 .. may cause the ruler to be too squashed, so maybe having ruler
    // segments of 0, 100, 200 (more spaced out) may be better
    const double lowerLimit = s_MIN_TICK_STEP * scaleFactor;
    while (tickStep < lowerLimit)
    {
        // ie 10, 100, 1000, 10000 etc.
        timePeriod *= 10;
        numSections = (markersPerSection * maxClockTime) / timePeriod;
        tickStep = rulerLength / numSections;
    }

    // Scale the ticks back down in the case where the gap between them is too large. Up to this
    // part, the scaling will be in base 10 but maybe the gap between 10000 and 20000 is too large.
    // In this case, the tickStep is halved so this goes down to 10000, 15000, 20000.
    const double upperLimit = s_MAX_TICK_STEP * scaleFactor;
    while (tickStep > upperLimit)
    {
        timePeriod /= 2;
        //RGP_ASSERT(timePeriod > 0);
        numSections = (markersPerSection * maxClockTime) / timePeriod;
        tickStep = rulerLength / numSections;
    }

    uint64_t count = 0;
    double clockLabel = 0;

    double scaleInc = timePeriod;
    scaleInc /= ((double)markersPerSection);

    // quick calculation to start the ruler at the start of the draw region
    const double startingTime = GetStartingTime(minVisibleTime, timePeriod, useTimePeriod);
    count = startingTime / (scaleInc * scaleFactor);
    clockLabel = scaleInc * scaleFactor * count;

    double xPos = rect.left() + (count * tickStep);

    // make sure the scale is valid before proceeding.
    if (scaleInc > 0)
    {
        while (clockLabel <= (maxVisibleTime + scaleInc))
        {
            const bool performDraw = (clockLabel >= startingTime);
            int top;

            // long marker
            if ((count % markersPerSection) == 0)
            {
                top = rect.bottom() - (markerUnitHeight * 4);

                // draw the text if visible
                if (performDraw == true)
                {
                    // Use the world transform to translate to the place we want the text to be,
                    // render the text, then reset the translation.
                    QTransform wt = pPainter->worldTransform();
                    wt.translate(xPos, 0);
                    pPainter->setWorldTransform(wt);
                    pPainter->drawText(1, top - (markerUnitHeight * 2), QtCommon::QtUtil::ClockToTimeUnit(clockLabel, unitType));
                    wt.translate(-xPos, 0);
                    pPainter->setWorldTransform(wt);
                }
            }

            // mid marker
            else if ((count % 10) == 0)
            {
                top = rect.bottom() - (markerUnitHeight * 3);
            }

            // short marker
            else
            {
                top = rect.bottom() - (markerUnitHeight * 2);
            }

            if (performDraw == true)
            {
                QTransform wt = pPainter->worldTransform();
                wt.translate(xPos, 0);
                pPainter->setWorldTransform(wt);
                pPainter->drawLine(1, top, 1, rect.bottom() - 1);
                wt.translate(-xPos, 0);
                pPainter->setWorldTransform(wt);
            }

            clockLabel += scaleInc * scaleFactor;
            count++;
            xPos += tickStep;
        }
    }
    pPainter->restore();
}

//-----------------------------------------------------------------------------
/// Constructor
/// \param config The widget's width
//-----------------------------------------------------------------------------
RulerWidget::RulerWidget(const RulerConfig& config) :
    m_minVisibleClk(0),
    m_maxVisibleClk(0)
{
    Update(config);

    // Ruler always on top
    setZValue(1);
}

//-----------------------------------------------------------------------------
/// Implementation of Qt's bounding volume for this item.
/// \return The item's bounding rectangle.
//-----------------------------------------------------------------------------
QRectF RulerWidget::boundingRect() const
{
    return QRectF(0, 0, m_config.width * m_config.scaleFactor, m_config.height * m_config.scaleFactor);
}

//-----------------------------------------------------------------------------
/// Implementation of Qt's shape for this item.
/// \return The item's shape.
//-----------------------------------------------------------------------------
QPainterPath RulerWidget::shape() const
{
    QPainterPath path;
    path.addRect(0, 0, m_config.width * m_config.scaleFactor, m_config.height * m_config.scaleFactor);
    return path;
}

//-----------------------------------------------------------------------------
/// Called when the ruler needs to change.
/// \param config The ruler's new config.
//-----------------------------------------------------------------------------
void RulerWidget::Update(const RulerConfig& config)
{
    memcpy(&m_config, &config, sizeof(m_config));
}

//-----------------------------------------------------------------------------
/// Called when the ruler needs to change.
/// \param config The ruler's new config.
//-----------------------------------------------------------------------------
void RulerWidget::UpdateVisibleBounds(uint64_t min, uint64_t max)
{
    m_minVisibleClk = min;
    m_maxVisibleClk = max;
}

//-----------------------------------------------------------------------------
/// Implementation of Qt's paint for this item.
/// \param pPainter The painter object to use.
/// \param pOption Provides style options for the item, such as its state, exposed area and its level-of-detail hints.
/// \param pWidget Points to the widget that is being painted on if specified.
//-----------------------------------------------------------------------------
void RulerWidget::paint(QPainter* pPainter, const QStyleOptionGraphicsItem *pOption, QWidget* pWidget)
{
    Q_UNUSED(pWidget);
    Q_UNUSED(pOption);

    // only draw the ruler if there's a time difference, otherwise there's nothing to show
    if (m_maxVisibleClk > m_minVisibleClk)
    {
        PaintRulerBackground(pPainter, boundingRect());
        PaintRuler(pPainter, boundingRect(), m_config.maxTime, m_minVisibleClk, m_maxVisibleClk, m_config.scaleFactor, m_config.timeToClockRatio, m_config.unitType);
    }
}
