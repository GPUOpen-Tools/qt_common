//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Implementation of a custom ListWidget that handles loss of focus
//=============================================================================

#include <QApplication>
#include <QPushButton>

#include <ListWidget.h>
#include <ArrowIconWidget.h>
#include <../Scaling/ScalingManager.h>
#include <../Util/CommonDefinitions.h>
#include <../Util/QtUtil.h>

//-----------------------------------------------------------------------------
/// Explicit constructor.
/// \param pParent Pointer to the parent widget
//-----------------------------------------------------------------------------
ListWidget::ListWidget(QWidget* pParent, ArrowIconWidget* pButton, bool hide) :
    QListWidget(pParent),
    m_pButton(pButton),
    m_hide(hide),
    m_pParent(pParent)
{
    connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(FocusChanged(QWidget*, QWidget*)));
}

//-----------------------------------------------------------------------------
/// Destructor.
//-----------------------------------------------------------------------------
ListWidget::~ListWidget()
{
    disconnect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(FocusChanged(QWidget*, QWidget*)));
}

//-----------------------------------------------------------------------------
/// Slot to handle what happens when the list box loses focus
/// \param pOld Pointer to the widget losing focus
/// \param pNow Pointer to the widget getting focus
//-----------------------------------------------------------------------------
void ListWidget::FocusChanged(QWidget* pOld, QWidget* pNow)
{
    Q_UNUSED(pOld);
    Q_UNUSED(pNow);

    if (m_hide)
    {
        // Hide the list widget
        hide();

        // Set the button icon to down arrow
        m_pButton->SetDirection(ArrowIconWidget::Direction::DownArrow);
    }
}

//-----------------------------------------------------------------------------
/// Implementation of the paintEvent method for this class.
/// \param pEvent The Qt paint event
//-----------------------------------------------------------------------------
void ListWidget::paintEvent(QPaintEvent* pEvent)
{
    Q_UNUSED(pEvent);

    // Calculate the position
    QRect rect = m_pButton->geometry();
    QPoint pos(0, 0);
    pos = m_pButton->mapTo(m_pParent, pos);
    pos.setY(pos.y() + rect.height());

    // Calculate the width and the height of the list widget
    int height = QtCommon::QtUtil::GetListWidgetHeight(this);
    int width = QtCommon::QtUtil::GetListWidgetWidth(this);

    // Set the font size
    QFont font = this->font();
    font.setPixelSize(s_BUTTON_PIXEL_FONT_SIZE * ScalingManager::Get().GetScaleFactor());
    setStyleSheet(s_LIST_WIDGET_STYLE.arg(font.pixelSize()));

    // Set geometry
    setGeometry(pos.x(), pos.y(), width + s_CHECK_BOX_WIDTH, height);

    // Call the base class implementation
    QListWidget::paintEvent(pEvent);
}
