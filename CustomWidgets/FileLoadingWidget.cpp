//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a file loading widget.
//=============================================================================

#include "FileLoadingWidget.h"

// use definition of PI from math.h
#define _USE_MATH_DEFINES

#include <math.h>
#include <QPainter>
#include <QTimer>
#include <QPaintEvent>
#include <QDebug>

#include <../Scaling/ScalingManager.h>

static const int s_ANIMATION_TIMER = 33;        // animation update rate, in ms

static const int s_BAR_WIDTH = 40;              // bar width, in pixels
static const int s_BAR_MAX_HEIGHT = 110;        // maximum bar height, in pixels
static const int s_BAR_MIN_HEIGHT = 50;         // minimum bar height, in pixels
static const int s_BAR_X_SPACING = 5;           // the spacing between bars, in pixels

static const int s_NUM_BARS = 5;                // number of animating bars
static const double s_ANIM_SPEED = M_PI / 70.0; // how fast the animation progresses

/// Class to encapsulate a single animating loading bar
class AnimBar
{
public:
    //-----------------------------------------------------------------------------
    /// Explicit constructor
    //-----------------------------------------------------------------------------
    explicit AnimBar(int xPos, int yPos, int index) :
        m_xPos(xPos),
        m_yPos(yPos),
        m_phase((M_PI * (index - (s_NUM_BARS / 2))) / 8.0),
        m_height(0)
    {
    }

    //-----------------------------------------------------------------------------
    /// Destructor
    //-----------------------------------------------------------------------------
    ~AnimBar()
    {
    }

    //-----------------------------------------------------------------------------
    /// Update a single loading bar. Use the absolute value of a sine wave for the
    /// bar height offset. A smoother method would be to use a sine squared wave.
    //-----------------------------------------------------------------------------
    void Update()
    {
        m_height = fabs(sin(m_phase));
        m_height *= (s_BAR_MAX_HEIGHT - s_BAR_MIN_HEIGHT);
        m_phase -= s_ANIM_SPEED;
    }

    //-----------------------------------------------------------------------------
    /// Paint a single animation bar
    /// \param pPainter Pointer to a QPainter object
    //-----------------------------------------------------------------------------
    void Paint(QPainter* pPainter, double scaleFactor)
    {
        qreal xPos = m_xPos * scaleFactor;
        qreal yPos = (m_yPos - s_BAR_MAX_HEIGHT + m_height) * scaleFactor;
        qreal width = s_BAR_WIDTH * scaleFactor;
        qreal height = (s_BAR_MAX_HEIGHT - m_height) * 2.0 * scaleFactor;
        pPainter->fillRect(QRectF(xPos, yPos, width, height), Qt::gray);
    }

private:
    int m_xPos;             ///< The animating bar x position
    int m_yPos;             ///< The animating bar y position

    double m_phase;         ///< The animating bar phase (how far it is from 0 degrees)
    double m_height;        ///< The animating bar height
};

/// Class to encapsulate a series of animating loading bars
class AnimBars
{
public:
    //-----------------------------------------------------------------------------
    /// Constructor
    //-----------------------------------------------------------------------------
    AnimBars()
    {
        int xPos = 0;
        int yPos = 0;
        for (int loop = 0; loop < s_NUM_BARS; loop++)
        {
            m_bars[loop] = new AnimBar(xPos, yPos, loop);
            xPos += s_BAR_WIDTH + s_BAR_X_SPACING;
        }
    }

    //-----------------------------------------------------------------------------
    /// Destructor
    //-----------------------------------------------------------------------------
    ~AnimBars()
    {
        for (int loop = 0; loop < s_NUM_BARS; loop++)
        {
            delete m_bars[loop];
        }
    }

    //-----------------------------------------------------------------------------
    /// Update a series of loading bars.
    //-----------------------------------------------------------------------------
    void Update()
    {
        for (int loop = 0; loop < s_NUM_BARS; loop++)
        {
            m_bars[loop]->Update();
        }
    }

    //-----------------------------------------------------------------------------
    /// Paint the animation bar series
    /// \param pPainter Pointer to a QPainter object
    //-----------------------------------------------------------------------------
    void Paint(QPainter* pPainter, double scaleFactor)
    {
        for (int loop = 0; loop < s_NUM_BARS; loop++)
        {
            m_bars[loop]->Paint(pPainter, scaleFactor);
        }
    }

private:
    AnimBar*    m_bars[s_NUM_BARS];         ///< The arrat of animating bars
};

//-----------------------------------------------------------------------------
/// Constructor
/// \param parent The animation widget's parent
//-----------------------------------------------------------------------------
FileLoadingWidget::FileLoadingWidget(QWidget* pParent) :
    QWidget(pParent),
    m_pUpdateTimer(nullptr)
{
    m_pUpdateTimer = new QTimer(this);
    connect(m_pUpdateTimer, &QTimer::timeout, this, &FileLoadingWidget::Animate);
    m_pUpdateTimer->start(s_ANIMATION_TIMER);
    m_pAnimBars = new AnimBars();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
FileLoadingWidget::~FileLoadingWidget()
{
    disconnect(m_pUpdateTimer, &QTimer::timeout, this, &FileLoadingWidget::Animate);
    m_pUpdateTimer->stop();
    delete m_pAnimBars;
}

//-----------------------------------------------------------------------------
/// Implementation of animation
//-----------------------------------------------------------------------------
void FileLoadingWidget::Animate()
{
    m_pAnimBars->Update();
    update();
}

//-----------------------------------------------------------------------------
/// Overridden paintEvent method.
/// \param event The paint event object
//-----------------------------------------------------------------------------
void FileLoadingWidget::paintEvent(QPaintEvent* pEvent)
{
    Q_UNUSED(pEvent);

    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.save();
    painter.setBrush(Qt::white);
    painter.drawRect(0, 0, width(), height()); //draws the background of widget

    double scaleFactor = ScalingManager::Get().GetScaleFactor();
    double xOffset = ((s_NUM_BARS * (s_BAR_WIDTH + s_BAR_X_SPACING)) / 2) * scaleFactor;
    painter.translate(rect().center() - QPoint(xOffset, 0));

    m_pAnimBars->Paint(&painter, scaleFactor);

    painter.restore();
    painter.end();
}
