//=============================================================================
/// Copyright (c) 2016-2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a double slider widget.
//=============================================================================

#include "DoubleSliderWidget.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QStylePainter>
#include <QStyleOptionSlider>
#include <QDebug>

#include <../Util/CommonDefinitions.h>

//-----------------------------------------------------------------------------
/// Constructor
/// \param pParent - The parent of Double Slider
//-----------------------------------------------------------------------------
DoubleSliderWidget::DoubleSliderWidget(QWidget* pParent) : QSlider(pParent)
{
    init();
}

//-----------------------------------------------------------------------------
/// Constructor
/// \param orientation - The type of orientation of slider
/// \param pParent - The parent of Double Slider
//-----------------------------------------------------------------------------
DoubleSliderWidget::DoubleSliderWidget(Qt::Orientation orientation, QWidget* pParent) : QSlider(orientation, pParent)
{
    init();
}

//-----------------------------------------------------------------------------
/// Implementation of init , intialises member variables to default values
//-----------------------------------------------------------------------------
void DoubleSliderWidget::init()
{
    m_lowerValue = 0;
    m_upperValue= 0;
    m_lowerPos = 0;
    m_upperPos = 0;
    m_offsetPos = 0;
    m_pos = 0;
    m_lastPressedSpan = DoubleSliderWidget::NoHandle;
    m_mainSpanControl= DoubleSliderWidget::LowerHandle;
    m_lowerPressedControl = QStyle::SC_None;
    m_upperPressedControl = QStyle::SC_None;
    m_handleMovement = DoubleSliderWidget::NoOverlapping;
    m_isFirstMovement = false;
    m_blockTracking = false;

    connect(this, SIGNAL(rangeChanged(int, int)), this, SLOT(updateRange(int, int)));
    connect(this, SIGNAL(sliderReleased()), this, SLOT(movePressedHandle()));

    setStyleSheet(gs_CUSTOM_SLIDER_STYLESHEET);

    setSpan(minimum(),maximum()); //sets the upper and lower position to range of a slider
}

//-----------------------------------------------------------------------------
/// Implementation of intilaizing style options of Double Slider.
/// \param pOption - The style option of Double Slider.
/// \param spanHandle - The span handle of Double Slider
//-----------------------------------------------------------------------------
void DoubleSliderWidget::initStyleOption(QStyleOptionSlider* pOption, DoubleSliderWidget::SpanHandle spanHandle) const
{
    QSlider::initStyleOption(pOption);
    pOption->sliderPosition = (spanHandle == DoubleSliderWidget::LowerHandle ? m_lowerPos : m_upperPos);
    pOption->sliderValue = (spanHandle == DoubleSliderWidget::LowerHandle ? m_lowerValue : m_upperValue);
}

//-----------------------------------------------------------------------------
/// Implementation of Pixel position to Range value for this item.
/// \param pixelPosition - The position of slider
/// \return The range value from the pixel value
//-----------------------------------------------------------------------------
int DoubleSliderWidget::pixelPosToRangeValue(int pixelPosition) const
{
    QStyleOptionSlider opt;
    initStyleOption(&opt);

    int sliderMin = 0;
    int sliderMax = 0;
    int sliderLength = 0;
    const QSlider* pSlider = this;
    const QRect grooveRect = pSlider->style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, pSlider);
    const QRect handleRect = pSlider->style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, pSlider);

    if (pSlider->orientation() == Qt::Horizontal)
    {
        sliderLength = handleRect.width();
        sliderMin = grooveRect.x();
        sliderMax = grooveRect.right() - sliderLength + 1;
    }
    else
    {
        sliderLength = handleRect.height();
        sliderMin = grooveRect.y();
        sliderMax = grooveRect.bottom() - sliderLength + 1;
    }

    return QStyle::sliderValueFromPosition(pSlider->minimum(), pSlider->maximum(), pixelPosition - sliderMin,
                                           sliderMax - sliderMin, opt.upsideDown);
}

//-----------------------------------------------------------------------------
/// Implementation of mouse press for this item.
/// \param mousePos - The position of mouse press
/// \param control - The control of Double Slider.
/// \param handle -  The handle for which mouse press event generated
//-----------------------------------------------------------------------------
void DoubleSliderWidget::handleMousePress(const QPoint& mousePos, QStyle::SubControl& control, int value, DoubleSliderWidget::SpanHandle spanHandle)
{
    QStyleOptionSlider opt;
    initStyleOption(&opt, spanHandle);

    const QStyle::SubControl oldControl = control;
    control = style()->hitTestComplexControl(QStyle::CC_Slider, &opt, mousePos, this);
    const QRect handleRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

    if (control == QStyle::SC_SliderHandle)
    {
        m_pos = value;
        m_offsetPos = pick(mousePos - handleRect.topLeft());
        m_lastPressedSpan = spanHandle;
        setSliderDown(true);
        emit sliderPressed(spanHandle);
    }

    if (control != oldControl)
    {
        update(handleRect);
    }
}

//-----------------------------------------------------------------------------
/// Implementation of setting up painter for this item.
/// \param pPainter - The painter used to draw this item
/// \param orientation - The type of orientation applied on this item
/// \param startX -  x co-ordinate of point1 in linear gradient
/// \param startY -  y co-ordinate of point1 in linear gradient
/// \param endX -  x co-ordinate of point2 in linear gradient
/// \param endY -  y co-ordinate of point2 in linear gradient
//-----------------------------------------------------------------------------
void DoubleSliderWidget::setupPainter(QPainter* pPainter, Qt::Orientation orientation, qreal startX, qreal startY, qreal endX, qreal endY) const
{
    QColor highlight = palette().color(QPalette::Highlight);
    QLinearGradient gradient(startX, startY, endX, endY);
    gradient.setColorAt(0, highlight.dark(120));
    gradient.setColorAt(1, highlight.light(108));
    pPainter->setBrush(gradient);

    if (orientation == Qt::Horizontal)
    {
        pPainter->setPen(QPen(highlight.dark(130), 0));
    }
    else
    {
        pPainter->setPen(QPen(highlight.dark(150), 0));
    }
}

//-----------------------------------------------------------------------------
/// Implementation of drawing span for this item.
/// \param pPainter - The painter used to draw span for this item
/// \param spanArea - The area of span
//-----------------------------------------------------------------------------
void DoubleSliderWidget::drawSpan(QStylePainter* pPainter, const QRect& spanArea) const
{
    QStyleOptionSlider opt;
    initStyleOption(&opt);

    const QSlider* pDoubleSlider = this;
    QRect grooveRect = pDoubleSlider->style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, pDoubleSlider);

    if (opt.orientation == Qt::Horizontal)
    {
        grooveRect.adjust(0, 2, -1, 0);
    }
    else
    {
        grooveRect.adjust(2, 0, 0, -1);
    }

    grooveRect.setHeight(1);

    // pen & brush
    pPainter->setPen(QPen(pDoubleSlider->palette().color(QPalette::Dark).light(), 0));

    if (opt.orientation == Qt::Horizontal)
    {
        setupPainter(pPainter, opt.orientation, grooveRect.center().x(), grooveRect.top(), grooveRect.center().x(), grooveRect.bottom());
    }
    else
    {
        setupPainter(pPainter, opt.orientation, grooveRect.left(), grooveRect.center().y(), grooveRect.right(), grooveRect.center().y());
    }

    // draw groove
    pPainter->drawRect(spanArea.intersected(grooveRect));
}

//-----------------------------------------------------------------------------
/// Implementation of drawing handle for this item.
/// \param pPainter - The painter used to draw span for this item
/// \param spanHandle - The handle of double slider
//-----------------------------------------------------------------------------
void DoubleSliderWidget::drawHandle(QStylePainter* pPainter, DoubleSliderWidget::SpanHandle spanHandle) const
{
    QStyleOptionSlider opt;
    initStyleOption(&opt, spanHandle);
    opt.subControls = QStyle::SC_SliderHandle;

    QStyle::SubControl pressed = (spanHandle == DoubleSliderWidget::LowerHandle ? m_lowerPressedControl : m_upperPressedControl);

    if (pressed == QStyle::SC_SliderHandle)
    {
        opt.activeSubControls = pressed;
        opt.state |= QStyle::State_Sunken;
    }

    pPainter->drawComplexControl(QStyle::CC_Slider, opt);
}

//-----------------------------------------------------------------------------
/// Implementation of trigger action for this item.
/// \param sliderAction - The slider action for this item
/// \param mainAction - This variable states wheter slider action is main action
//-----------------------------------------------------------------------------
void DoubleSliderWidget::triggerAction(QAbstractSlider::SliderAction sliderAction, bool mainAction)
{
    int sliderValue = 0;
    bool noSliderAction = false;
    bool upSliderAction = false;
    const int minValue = minimum();
    const int maxValue = maximum();
    const DoubleSliderWidget::SpanHandle altControl = (m_mainSpanControl == DoubleSliderWidget::LowerHandle ? DoubleSliderWidget::UpperHandle : DoubleSliderWidget::LowerHandle);

    m_blockTracking = true;

    switch (sliderAction)
    {
        case QAbstractSlider::SliderSingleStepAdd:
            if ((mainAction && m_mainSpanControl == DoubleSliderWidget::UpperHandle) || (!mainAction && altControl == DoubleSliderWidget::UpperHandle))
            {
                sliderValue = qBound(minValue, m_upperValue + singleStep(), maxValue);
                upSliderAction = true;
                break;
            }

            sliderValue = qBound(minValue, m_lowerValue + singleStep(), maxValue);
            break;

        case QAbstractSlider::SliderSingleStepSub:
            if ((mainAction && m_mainSpanControl == DoubleSliderWidget::UpperHandle) || (!mainAction && altControl == DoubleSliderWidget::UpperHandle))
            {
                sliderValue = qBound(minValue, m_upperValue - singleStep(), maxValue);
                upSliderAction = true;
                break;
            }

            sliderValue = qBound(minValue, m_lowerValue - singleStep(), maxValue);
            break;

        case QAbstractSlider::SliderToMinimum:
            sliderValue = minValue;

            if ((mainAction && m_mainSpanControl == DoubleSliderWidget::UpperHandle) || (!mainAction && altControl == DoubleSliderWidget::UpperHandle))
            {
                upSliderAction = true;
            }

            break;

        case QAbstractSlider::SliderToMaximum:
            sliderValue = maxValue;

            if ((mainAction && m_mainSpanControl == DoubleSliderWidget::UpperHandle) || (!mainAction && altControl == DoubleSliderWidget::UpperHandle))
            {
                upSliderAction = true;
            }

            break;

        case QAbstractSlider::SliderMove:
            if ((mainAction && m_mainSpanControl == DoubleSliderWidget::UpperHandle) || (!mainAction && altControl == DoubleSliderWidget::UpperHandle))
            {
                upSliderAction = true;
            }
            noSliderAction = true;
            break;
        case QAbstractSlider::SliderNoAction:
            noSliderAction = true;
            break;

        default:
            qWarning("DoubleSliderWidget::triggerAction: Unknown action");
            break;
    }

    if (!noSliderAction)
    {
        if(upSliderAction)
        {
            if (m_handleMovement == DoubleSliderWidget::NoCrossing)
            {
                sliderValue = qMax(sliderValue, m_lowerValue);
            }
            else if (m_handleMovement == DoubleSliderWidget::NoOverlapping)
            {
                sliderValue = qMax(sliderValue, m_lowerValue + 1);
            }

            if (m_handleMovement == DoubleSliderWidget::FreeMovement && sliderValue < m_lowerValue)
            {
                swapControls();
                setLowerPosition(sliderValue);
            }
            else
            {
                setUpperPosition(sliderValue);
            }
        }
        else
        {
            if (m_handleMovement == DoubleSliderWidget::NoCrossing)
            {
                sliderValue = qMin(sliderValue, m_upperValue);
            }
            else if (m_handleMovement == DoubleSliderWidget::NoOverlapping)
            {
                sliderValue = qMin(sliderValue, m_upperValue - 1);
            }

            if (m_handleMovement == DoubleSliderWidget::FreeMovement && sliderValue > m_upperValue)
            {
                swapControls();
                setUpperPosition(sliderValue);
            }
            else
            {
                setLowerPosition(sliderValue);
            }
        }
    }

    m_blockTracking = false;
    setLowerValue(m_lowerPos);
    setUpperValue(m_upperPos);
}

//-----------------------------------------------------------------------------
/// Implementation of swap controls.
//-----------------------------------------------------------------------------
void DoubleSliderWidget::swapControls()
{
    qSwap(m_lowerValue, m_upperValue);
    qSwap(m_lowerPressedControl, m_upperPressedControl);

    m_lastPressedSpan = (m_lastPressedSpan == DoubleSliderWidget::LowerHandle ? DoubleSliderWidget::UpperHandle : DoubleSliderWidget::LowerHandle);
    m_mainSpanControl = (m_mainSpanControl == DoubleSliderWidget::LowerHandle ? DoubleSliderWidget::UpperHandle : DoubleSliderWidget::LowerHandle);
}

//-----------------------------------------------------------------------------
/// Implementation of update range for this item.
/// \param minValue - The min value of double slider
/// \param maxValue - The max value of double slider
//-----------------------------------------------------------------------------
void DoubleSliderWidget::updateRange(int minValue, int maxValue)
{
    Q_UNUSED(minValue);
    Q_UNUSED(maxValue);

    // setSpan() takes care of keeping span in range
    setSpan(m_lowerValue, m_upperValue);
}

//-----------------------------------------------------------------------------
/// Implementation of move press handle
//-----------------------------------------------------------------------------
void DoubleSliderWidget::movePressedHandle()
{
    switch (m_lastPressedSpan)
    {
        case DoubleSliderWidget::LowerHandle:
            if (m_lowerPos != m_lowerValue)
            {
                bool mainAction = (m_mainSpanControl == DoubleSliderWidget::LowerHandle);
                triggerAction(QAbstractSlider::SliderMove, mainAction);
            }
            break;

        case DoubleSliderWidget::UpperHandle:
            if (m_upperPos != m_upperValue)
            {
                bool mainAction = (m_mainSpanControl == DoubleSliderWidget::UpperHandle);
                triggerAction(QAbstractSlider::SliderMove, mainAction);
            }
            break;

        default:
            break;
    }
}

//-----------------------------------------------------------------------------
/// Implementation of handle movement mode for this widget
//-----------------------------------------------------------------------------
DoubleSliderWidget::HandleMovementMode DoubleSliderWidget::handleMovementMode() const
{
    return m_handleMovement;
}

//-----------------------------------------------------------------------------
/// Implementation of set handle movement mode for this widget
/// \param handleMovement - The mode to be set
//-----------------------------------------------------------------------------
void DoubleSliderWidget::setHandleMovementMode(DoubleSliderWidget::HandleMovementMode handleMovement)
{
    m_handleMovement = handleMovement;
}

//-----------------------------------------------------------------------------
/// Return's of lower value for this widget
/// \return - The lower value of slider
//-----------------------------------------------------------------------------
int DoubleSliderWidget::lowerValue() const
{
    return qMin(m_lowerValue, m_upperValue);
}

//-----------------------------------------------------------------------------
/// Implementation of set lower value for the Double Slider
/// \param lowerValue - The lower value of Double Slider
//-----------------------------------------------------------------------------
void DoubleSliderWidget::setLowerValue(int lowerValue)
{
    setSpan(lowerValue, m_upperValue);
}

//-----------------------------------------------------------------------------
/// Return's of upper value for this widget
/// \return - The upper value of slider
//-----------------------------------------------------------------------------
int DoubleSliderWidget::upperValue() const
{
    return qMax(m_lowerValue, m_upperValue);
}

//-----------------------------------------------------------------------------
/// Implementation of set upper value for the Double Slider
/// \param upperValue - The upper value of Double Slider
//-----------------------------------------------------------------------------
void DoubleSliderWidget::setUpperValue(int upperValue)
{
    setSpan(m_lowerValue, upperValue);
}

//-----------------------------------------------------------------------------
/// Implementation of set Span for the Double Slider
/// \param lowerValue - The lower value of Double Slider
/// \param upperValue - The upper value of Double Slider
//-----------------------------------------------------------------------------
void DoubleSliderWidget::setSpan(int lowerValue, int upperValue)
{
    const int lowerHandleValue = qBound(minimum(), qMin(lowerValue, upperValue), maximum());
    const int upperHandleValue = qBound(minimum(), qMax(lowerValue, upperValue), maximum());

    if (lowerHandleValue != m_lowerValue || upperHandleValue != m_upperValue)
    {
        if (lowerHandleValue != m_lowerValue)
        {
            m_lowerValue = lowerHandleValue;
            m_lowerPos = lowerHandleValue;
            emit lowerValueChanged(lowerHandleValue);
        }

        if (upperHandleValue != m_upperValue)
        {
            m_upperValue = upperHandleValue;
            m_upperPos = upperHandleValue;
            emit upperValueChanged(upperHandleValue);
        }

        emit spanChanged(m_lowerValue, m_upperValue);
        update();
    }
}

//-----------------------------------------------------------------------------
/// Return's of lower position for this widget
/// \return - The lower position of slider
//-----------------------------------------------------------------------------
int DoubleSliderWidget::lowerPosition() const
{
    return m_lowerPos;
}

//-----------------------------------------------------------------------------
/// Implementation of set lower position for the Double Slider
/// \param lowerPos - The lower position of Double Slider
//-----------------------------------------------------------------------------
void DoubleSliderWidget::setLowerPosition(int lowerPos)
{
    if (m_lowerPos != lowerPos)
    {
        m_lowerPos = lowerPos;

        if (!hasTracking())
        {
            update();
        }

        if (isSliderDown())
        {
            emit lowerPositionChanged(lowerPos);
        }

        if (hasTracking() && !m_blockTracking)
        {
            bool mainAction = (m_mainSpanControl == DoubleSliderWidget::LowerHandle);
            triggerAction(SliderMove, mainAction);
        }
    }
}

//-----------------------------------------------------------------------------
/// Return's of upper position for this widget
/// \return - The upper position of slider
//-----------------------------------------------------------------------------
int DoubleSliderWidget::upperPosition() const
{
    return m_upperPos;
}

//-----------------------------------------------------------------------------
/// Implementation of set upper position for the Double Slider
/// \param upperPos - The upper position of Double Slider
//-----------------------------------------------------------------------------
void DoubleSliderWidget::setUpperPosition(int upperPos)
{
    if (m_upperPos != upperPos)
    {
        m_upperPos = upperPos;

        if (!hasTracking())
        {
            update();
        }

        if (isSliderDown())
        {
            emit upperPositionChanged(upperPos);
        }

        if (hasTracking() && !m_blockTracking)
        {
            bool mainAction = (m_mainSpanControl == DoubleSliderWidget::UpperHandle);
            triggerAction(SliderMove, mainAction);
        }
    }
}

//-----------------------------------------------------------------------------
/// Implementation of Qt's key press event for the Double Slider
/// \param pEvent - The key event of Double slider
//-----------------------------------------------------------------------------
void DoubleSliderWidget::keyPressEvent(QKeyEvent* pEvent)
{
    QSlider::keyPressEvent(pEvent);

    bool mainAction = true;
    SliderAction sliderAction = SliderNoAction;

    switch (pEvent->key())
    {
        case Qt::Key_Left:
            mainAction   = (orientation() == Qt::Horizontal);
            sliderAction = !invertedAppearance() ? SliderSingleStepSub : SliderSingleStepAdd;
            break;

        case Qt::Key_Right:
            mainAction   = (orientation() == Qt::Horizontal);
            sliderAction = !invertedAppearance() ? SliderSingleStepAdd : SliderSingleStepSub;
            break;

        case Qt::Key_Up:
            mainAction   = (orientation() == Qt::Vertical);
            sliderAction = invertedControls() ? SliderSingleStepSub : SliderSingleStepAdd;
            break;

        case Qt::Key_Down:
            mainAction   = (orientation() == Qt::Vertical);
            sliderAction = invertedControls() ? SliderSingleStepAdd : SliderSingleStepSub;
            break;

        case Qt::Key_Home:
            mainAction   = (m_mainSpanControl == DoubleSliderWidget::LowerHandle);
            sliderAction = SliderToMinimum;
            break;

        case Qt::Key_End:
            mainAction   = (m_mainSpanControl == DoubleSliderWidget::UpperHandle);
            sliderAction = SliderToMaximum;
            break;

        default:
            pEvent->ignore();
            break;
    }

    if (sliderAction != SliderNoAction)
    {
        triggerAction(sliderAction, mainAction);
    }
}

//-----------------------------------------------------------------------------
/// Implementation of Qt's mouse press event for the Double Slider
/// \param pEvent - The mouse event of Double slider
//-----------------------------------------------------------------------------
void DoubleSliderWidget::mousePressEvent(QMouseEvent* pEvent)
{
    if (minimum() == maximum() || (pEvent->buttons() ^ pEvent->button()))
    {
        pEvent->ignore();
        return;
    }

    handleMousePress(pEvent->pos(), m_upperPressedControl, m_upperValue, DoubleSliderWidget::UpperHandle);

    if (m_upperPressedControl != QStyle::SC_SliderHandle)
    {
        handleMousePress(pEvent->pos(), m_lowerPressedControl, m_lowerValue, DoubleSliderWidget::LowerHandle);
    }

    m_isFirstMovement = true;
    pEvent->accept();
}

//-----------------------------------------------------------------------------
/// Implementation of Qt's mouse press event for the Double Slider
/// \param pEvent - The mouse move event of Double slider
//-----------------------------------------------------------------------------
void DoubleSliderWidget::mouseMoveEvent(QMouseEvent* pEvent)
{
    if (m_lowerPressedControl != QStyle::SC_SliderHandle && m_upperPressedControl != QStyle::SC_SliderHandle)
    {
        pEvent->ignore();
        return;
    }

    QStyleOptionSlider opt;
    initStyleOption(&opt);
    const int pixelMetric = style()->pixelMetric(QStyle::PM_MaximumDragDistance, &opt, this);
    int newPosition = pixelPosToRangeValue(pick(pEvent->pos()) - m_offsetPos);

    if (pixelMetric >= 0)
    {
        const QRect sliderRect = rect().adjusted(-pixelMetric, -pixelMetric, pixelMetric, pixelMetric);

        if (!sliderRect.contains(pEvent->pos()))
        {
            newPosition = m_pos;
        }
    }

    // pick the preferred handle on the first movement
    if (m_isFirstMovement)
    {
        if (m_lowerValue == m_upperValue)
        {

            if (newPosition < lowerValue())
            {
                swapControls();
                m_isFirstMovement = false;
            }

        }
        else
        {
            m_isFirstMovement = false;
        }
    }

    if (m_lowerPressedControl == QStyle::SC_SliderHandle)
    {
        if (m_handleMovement == NoCrossing)
        {
            newPosition = qMin(newPosition, upperValue());
        }
        else if (m_handleMovement == NoOverlapping)
        {
            newPosition = qMin(newPosition, upperValue() - 1);
        }

        if (m_handleMovement == FreeMovement && newPosition > m_upperValue)
        {
            swapControls();
            setUpperPosition(newPosition);
        }
        else
        {
            setLowerPosition(newPosition);
        }
    }
    else if (m_upperPressedControl == QStyle::SC_SliderHandle)
    {
        if (m_handleMovement == NoCrossing)
        {
            newPosition = qMax(newPosition, lowerValue());
        }
        else if (m_handleMovement == NoOverlapping)
        {
            newPosition = qMax(newPosition, lowerValue() + 1);
        }

        if (m_handleMovement == FreeMovement && newPosition < m_lowerValue)
        {
            swapControls();
            setLowerPosition(newPosition);
        }
        else
        {
            setUpperPosition(newPosition);
        }
    }

    pEvent->accept();
}

//-----------------------------------------------------------------------------
/// Implementation of Qt's mouse release event for the Double Slider
/// \param pEvent - The mouse event of Double slider
//-----------------------------------------------------------------------------
void DoubleSliderWidget::mouseReleaseEvent(QMouseEvent* pEvent)
{
    QSlider::mouseReleaseEvent(pEvent);
    setSliderDown(false);
    m_lowerPressedControl = QStyle::SC_None;
    m_upperPressedControl = QStyle::SC_None;
    emit sliderReleased(m_lowerValue, m_upperValue);
    update();
}

//-----------------------------------------------------------------------------
/// Implementation of Qt's paint event for the Double Slider
/// \param pEvent - The mouse event of Double slider
//-----------------------------------------------------------------------------
void DoubleSliderWidget::paintEvent(QPaintEvent* pEvent)
{
    Q_UNUSED(pEvent);
    QStylePainter painter(this);

    // groove & ticks
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    opt.sliderValue = 0;
    opt.sliderPosition = 0;
    opt.subControls = QStyle::SC_SliderGroove | QStyle::SC_SliderTickmarks;
    painter.drawComplexControl(QStyle::CC_Slider, opt);

    // handle rects
    opt.sliderPosition = m_lowerPos;
    const QRect lowerHandleRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    const int lowerHandleValue  = pick(lowerHandleRect.center());
    opt.sliderPosition = m_upperPos;
    const QRect upperHandleRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    const int upperHandleValue  = pick(upperHandleRect.center());

    // span
    const int minValue = qMin(lowerHandleValue, upperHandleValue);
    const int maxValue = qMax(lowerHandleValue, upperHandleValue);
    const QPoint centerPoint = QRect(lowerHandleRect.center(), upperHandleRect.center()).center();
    QRect spanRect;

    if (orientation() == Qt::Horizontal)
    {
        spanRect = QRect(QPoint(minValue, centerPoint.y() - 2), QPoint(maxValue, centerPoint.y() + 1));
    }
    else
    {
        spanRect = QRect(QPoint(centerPoint.x() - 2, minValue), QPoint(centerPoint.x() + 1, maxValue));
    }

    drawSpan(&painter, spanRect);

    // handles
    switch (m_lastPressedSpan)
    {
        case DoubleSliderWidget::LowerHandle:
            drawHandle(&painter, DoubleSliderWidget::UpperHandle);
            drawHandle(&painter, DoubleSliderWidget::LowerHandle);
            break;

        case DoubleSliderWidget::UpperHandle:
        default:
            drawHandle(&painter, DoubleSliderWidget::LowerHandle);
            drawHandle(&painter, DoubleSliderWidget::UpperHandle);
            break;
    }
}
