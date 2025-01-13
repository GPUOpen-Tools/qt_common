//=============================================================================
/// Copyright (c) 2016-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for a donut pie widget.
///
/// This is like a pie chart, but with a hole in the center.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_DONUT_PIE_WIDGET_H_
#define QTCOMMON_CUSTOM_WIDGETS_DONUT_PIE_WIDGET_H_

#include <QWidget>

/// A donut pie widget.
class DonutPieWidget : public QWidget
{
    Q_OBJECT

public:
    /// Explicit constructor.
    ///
    /// @param [in] parent The parent widget
    explicit DonutPieWidget(QWidget* parent);

    /// Virtual destructor.
    virtual ~DonutPieWidget();

    /// @brief Custom sizeHint implementation that supports DPI scaling.
    ///
    /// @return A default sizeHint since the size of this widget can grow to fit
    /// the space allowed by the layout.
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

    /// @brief Set the number of segments for this control. This is the number of
    /// unique data elements to be shown in this control
    ///
    /// @param [in] numSegments the number of segments needed
    void SetNumSegments(unsigned int numSegments);

    /// @brief Set the value for the given index for the control.
    ///
    /// @param [in] index the index whose value is to change
    /// @param [in] value the new value to use
    void SetIndexValue(unsigned int index, qreal value);

    /// @brief Set the fill color for the given index for the control.
    ///
    /// @param [in] index the index whose color is to change
    /// @param [in] fillColor the color to use
    void SetIndexColor(unsigned int index, const QColor& fillColor);

    /// @brief Set the text to be displayed in the pie segment.
    ///
    /// @param [in] index the index whose text is to change
    /// @param [in] text the text to be shown
    void SetIndexText(unsigned int index, const QString& text);

    /// @brief Set how wide the donut section should be.
    ///
    /// @param [in] arcWidth the width of the donut arc
    void SetArcWidth(qreal arcWidth);

    /// @brief Set the first line of text inside the donut
    ///
    /// @param [in] text Text to set
    void SetTextLineOne(const QString& text);

    /// @brief Set the second line of text inside the donut
    ///
    /// @param [in] text Text to set
    void SetTextLineTwo(const QString& text);

    /// @brief Set the size of the font parameters for the donut.
    ///
    /// @param [in] value_font_size  The size of the font used to display the numeric value, in pixels.
    /// @param [in] text_font_size   The size of the font used to display the donut text, in pixels.
    void SetFontSizes(int value_font_size, int text_font_size);

    /// @brief Set the width/height size of the donut.
    ///
    /// @param [in] size             The width/height of the donut.
    void SetSize(int size);

protected:
    /// @brief Implementation of Qt's paint for this item.
    ///
    /// @param [in] paint_event The paint event.
    virtual void paintEvent(QPaintEvent* paint_event) Q_DECL_OVERRIDE;

private:
    class SliceData
    {
    public:
        SliceData()
            : value_(0)
            , fill_color_(Qt::black)
            , slice_text_()
        {
        }
        ~SliceData()
        {
        }

        qreal   value_;       ///< current control value to represent
        QColor  fill_color_;  ///< color used to fill the value part of the arc.
        QString slice_text_;  ///< Additional text description
    };

    /// Default width and height of the widget.
    const int kDefaultWidthAndHeight_ = 200;

    /// Default font pixel-size for writing the value.
    const int kValuePixelFontSize_ = 36;

    /// Default font pixel-size for writing the text.
    const int kTextPixelFontSize_ = 14;

    /// Vector of slice data.
    QVector<SliceData> slices_;

    unsigned int num_segments_;     ///< Number of segments (values) in the pie control.
    qreal        arc_width_;        ///< Width of the control arc, in pixels. This is used as the size of the pen used to draw the arc.
    QString      text_line_one_;    ///< Text in the center of the donut.
    QString      text_line_two_;    ///< Text in the center of the donut.
    int          size_;             ///< The width / height of the donut.
    int          value_font_size_;  ///< The font size used to display the donut value, in pixels.
    int          text_font_size_;   ///< The font size used to display the donut text, in pixels.
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_DONUT_PIE_WIDGET_H_
