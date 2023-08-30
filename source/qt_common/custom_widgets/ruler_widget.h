//=============================================================================
/// Copyright (c) 2016-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header file for a ruler.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_RULER_WIDGET_H_
#define QTCOMMON_CUSTOM_WIDGETS_RULER_WIDGET_H_

#include <QGraphicsItem>

#include "qt_common/utils/common_definitions.h"

class QColor;

struct RulerConfig
{
    RulerConfig()
        : height(0)
        , width(0)
        , max_time(0)
        , time_to_clock_ratio(0)
        , unit_type(0)
    {
    }

    int      height;               ///< Height of the ruler widget
    int      width;                ///< Width of the ruler widget
    uint64_t max_time;             ///< Maximum time value of the trace
    double   time_to_clock_ratio;  ///< Cache time to clock ratio
    int      unit_type;            ///< Displaying clk, ns, us, or ms
};

/// Class that describes a QGraphicsItem ruler.
class RulerWidget : public QGraphicsItem
{
public:
    /// Static utility function which holds ruler paint logic.
    /// \param painter Pointer to QPainter
    /// \param rect The rect encompassing the whole ruler
    /// \param max_time The maximum clock value
    /// \param min_visible_clk The minimum visible clock value
    /// \param max_visible_clk The maximum visible clock value
    /// \param time_to_clk_ratio The time to clock ratio
    /// \param unit_type Working with clk, ns, us, or ms
    static void PaintRuler(QPainter*     painter,
                           const QRectF& rect,
                           uint64_t      max_time,
                           uint64_t      min_visible_clk,
                           uint64_t      max_visible_clk,
                           double        time_to_clk_ratio,
                           int           unit_type,
                           bool          use_time_period = true);

    /// Static utility function to draw the ruler background. Allows external
    /// implementations (rulered table headers) to draw the correct background
    /// region.
    /// \param painter Pointer to QPainter
    /// \param rect The rect encompassing the whole ruler
    static void PaintRulerBackground(QPainter* painter, const QRectF& rect);

    /// Constructor
    /// \param config Ruler configuration
    RulerWidget(const RulerConfig& config);

    /// Virtual destructor
    virtual ~RulerWidget()
    {
    }

    /// Overridden implementation of Qt's bounding rect for this item.
    /// \return The item's bounding rectangle.
    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;

    /// Implementation of Qt's shape for this item.
    /// \return The item's shape.
    virtual QPainterPath shape() const Q_DECL_OVERRIDE;

    /// Implementation of Qt's paint for this item.
    /// \param painter The painter object to use.
    /// \param option Provides style options for the item, such as its state, exposed area and its level-of-detail hints.
    /// \param widget Points to the widget that is being painted on if specified.
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) Q_DECL_OVERRIDE;

    /// Called when the ruler needs to change.
    /// \param config The ruler's new config.
    void Update(const RulerConfig& config);

    /// Called when the ruler needs to change.
    /// \param min_clk The ruler's new minimum clock value.
    /// \param max_clk The ruler's new maximum clock value.
    void UpdateVisibleBounds(uint64_t min_clk, uint64_t max_clk);

private:
    /// Get the start time of the ruler. A different value is needed for ruler
    /// widgets attached to GraphicScene objects and QTreeView headers.
    /// \param min_visible_time The minimum time visible on the ruler
    /// \param time_period The time period between each marker. Will depend on the
    ///  zoom level
    /// \param use_time_period A flag indicating whether the time period should be
    ///  used in the calculation
    static double GetStartingTime(double min_visible_time, uint64_t time_period, bool use_time_period);

    /// Ruler config/setup
    RulerConfig config_;

    ///< Track the left-most visible clock
    uint64_t minimum_visible_clock_;

    /// Track the right-most visible clock
    uint64_t maximum_visible_clock_;
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_RULER_WIDGET_H_
