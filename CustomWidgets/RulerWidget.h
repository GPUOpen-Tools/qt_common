//=============================================================================
/// Copyright (c) 2016-2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header file for a ruler.
//=============================================================================

#ifndef _RULER_WIDGET_H_
#define _RULER_WIDGET_H_

#include <QGraphicsItem>

class QColor;

struct RulerConfig
{
    int       height;              ///< Height of the ruler widget
    int       width;               ///< Width of the ruler widget
    uint64_t  maxTime;             ///< Maximum time value of the trace
    double    scaleFactor;         ///< Current scaling for this ruler
    double    timeToClockRatio;    ///< Cache time to clock ratio
    int       unitType;            ///< Displaying clk, ns, us, or ms
};

/// Class that describes a QGraphicsItem ruler.
class RulerWidget : public QGraphicsItem
{
public:
    static void PaintRuler(QPainter* pPainter, const QRectF& rect, uint64_t maxTime, uint64_t minVisibleClk, uint64_t maxVisibleClk, double scaleFactor, double timeToClockRatio, int unitType);

    RulerWidget(const RulerConfig& config);
	virtual ~RulerWidget() {}

    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;
    virtual QPainterPath shape() const Q_DECL_OVERRIDE;
    virtual void paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pItem, QWidget* pWidget) Q_DECL_OVERRIDE;

    void Update(const RulerConfig& config);

    void UpdateVisibleBounds(uint64_t min, uint64_t max);

private:
    RulerConfig     m_config;           ///< Ruler config/setup
    uint64_t        m_minVisibleClk;    ///< Track the left-most visible clock
    uint64_t        m_maxVisibleClk;    ///< Track the right-most visible clock
};

#endif // _RULER_WIDGET_H_
