//=============================================================================
/// Copyright (c) 2016-2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for the Double Slider
//=============================================================================
#ifndef _DOUBLE_SLIDER_WIDGET_H_
#define _DOUBLE_SLIDER_WIDGET_H_

#include <QSlider>
#include <QStyle>
#include <QObject>

class QStylePainter;
class QStyleOptionSlider;

class DoubleSliderWidget : public QSlider
{
    Q_OBJECT

    Q_PROPERTY(int lowerValue READ lowerValue WRITE setLowerValue)
    Q_PROPERTY(int upperValue READ upperValue WRITE setUpperValue)
    Q_PROPERTY(int lowerPosition READ lowerPosition WRITE setLowerPosition)
    Q_PROPERTY(int upperPosition READ upperPosition WRITE setUpperPosition)
    Q_PROPERTY(HandleMovementMode handleMovementMode READ handleMovementMode WRITE setHandleMovementMode)
    Q_ENUMS(HandleMovementMode)

public:
    explicit DoubleSliderWidget(QWidget* pParent = nullptr);
    explicit DoubleSliderWidget(Qt::Orientation orientation, QWidget* pParent = nullptr);
    //Destructor
    virtual ~DoubleSliderWidget()
    {

    }

    enum HandleMovementMode
    {
        FreeMovement,
        NoCrossing,
        NoOverlapping
    };

    enum SpanHandle
    {
        NoHandle,
        LowerHandle,
        UpperHandle
    };

    void init();
    void initStyleOption(QStyleOptionSlider* pOption, DoubleSliderWidget::SpanHandle spanHandle = DoubleSliderWidget::UpperHandle) const;

    HandleMovementMode handleMovementMode() const;
    void setHandleMovementMode(HandleMovementMode handleMovement);

    int lowerValue() const;
    int upperValue() const;

    int lowerPosition() const;
    int upperPosition() const;

    int pick(const QPoint& handlePoint) const
    {
        return orientation() == Qt::Horizontal ? handlePoint.x() : handlePoint.y();
    }
    int pixelPosToRangeValue(int pixelPosition) const;

    void handleMousePress(const QPoint& mousePos, QStyle::SubControl& control, int value, DoubleSliderWidget::SpanHandle spanHandle);
    void drawHandle(QStylePainter* pPainter, DoubleSliderWidget::SpanHandle spanHandle) const;
    void setupPainter(QPainter* pPainter, Qt::Orientation orientation, qreal startX, qreal startY, qreal endX, qreal endY) const;
    void drawSpan(QStylePainter* pPainter, const QRect& spanArea) const;

    void triggerAction(QAbstractSlider::SliderAction sliderAction, bool mainAction);
    void swapControls();

public slots:
    void setLowerValue(int lowerValue);
    void setUpperValue(int upperValue);
    void setSpan(int lowerValue, int upperValue);

    void setLowerPosition(int lowerPos);
    void setUpperPosition(int upperPos);

    void updateRange(int minValue, int maxValue);
    void movePressedHandle();

signals:
    void spanChanged(int lowerValue, int upperValue);
    void lowerValueChanged(int lowerValue);
    void upperValueChanged(int upperValue);

    void lowerPositionChanged(int lowerPos);
    void upperPositionChanged(int upperPos);

    void sliderPressed(SpanHandle sliderHandle);
    void sliderReleased(int lowerValue, int upperValue);

protected:
    // Implementations for Qt
    virtual void keyPressEvent(QKeyEvent* pEvent);
    virtual void mousePressEvent(QMouseEvent* pEvent);
    virtual void mouseMoveEvent(QMouseEvent* pEvent);
    virtual void mouseReleaseEvent(QMouseEvent* pEvent);
    virtual void paintEvent(QPaintEvent* pEvent);

private:
    int m_lowerValue;             ///< lower value of the slider
    int m_upperValue;             ///< upper value of the slider
    int m_lowerPos;               ///< lower position of the slider
    int m_upperPos;               ///< upper position of the slider
    int m_offsetPos;              ///< offset to the position
    int m_pos;                    ///< position of slider
    DoubleSliderWidget::SpanHandle m_lastPressedSpan;            ///< last pressed state of span handle
    DoubleSliderWidget::SpanHandle m_mainSpanControl;            ///< main control of span handle
    QStyle::SubControl m_lowerPressedControl;                       ///< style of lower pressed
    QStyle::SubControl m_upperPressedControl;                       ///< style of upper pressed
    DoubleSliderWidget::HandleMovementMode m_handleMovement;     ///< movement of handle
    bool m_isFirstMovement;                                         ///< states first movement
    bool m_blockTracking;                                           ///< states tracking of the movement
};

#endif // _DOUBLE_SLIDER_WIDGET_H_
