//=============================================================================
/// Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a navigation list widget
//=============================================================================

#include "NavigationListWidget.h"

#include <QDebug>
#include <QMouseEvent>
#include <QFont>

const uint32_t s_BASE_WIDTH = 200;

const static QString s_MAIN_LIST_WIDGET_STYLESHEET
(
    "QListWidget"
    "{"
	    "background: rgb(240, 240, 240);"
	    "border-right: 2px groove gray;"
	    "outline: none;"
	    "border: none;"
	    "font: 8pt;"
    "}"

    "QListWidget::item:hover"
    "{"
	    "background: rgb(214, 214, 214);"
    "}"

    "QListWidget::item:selected"
    "{"
	    "background: rgb(143, 193, 231);"
    "}"

    "QListWidget::item"
    "{"
	    "border-bottom: none;"
	    "padding: 7px;"
	    "color: rgb(51, 51, 51);"
    "}"
);

//-----------------------------------------------------------------------------
/// Constructor.
/// \param pParent The view's parent.
//-----------------------------------------------------------------------------
NavigationListWidget::NavigationListWidget(QWidget* pParent) :
    QListWidget(pParent)
{
    setMouseTracking(true);
    setStyleSheet(s_MAIN_LIST_WIDGET_STYLESHEET);

    setMaximumWidth(s_BASE_WIDTH);
    setMinimumWidth(s_BASE_WIDTH);

    QFont font = QWidget::font();
    font.setPointSize(8);
    font.setBold(true);
    setFont(font);
}

//-----------------------------------------------------------------------------
/// Handle mouse movement
//-----------------------------------------------------------------------------
void NavigationListWidget::mouseMoveEvent(QMouseEvent* pEvent)
{
    QListWidget::mouseMoveEvent(pEvent);

    if (pEvent != nullptr)
    {
        const QPoint pos = pEvent->pos();
        const QModelIndex modelIndex = indexAt(pos);

        if (modelIndex.isValid())
        {
            setCursor(Qt::PointingHandCursor);
        }
        else
        {
            setCursor(Qt::ArrowCursor);
        }
    }
}