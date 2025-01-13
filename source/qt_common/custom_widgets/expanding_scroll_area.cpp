//=============================================================================
/// Copyright (c) 2020-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief A QScrollArea derived class that increases the width when horizontal
/// scrollbar is turned off or increases the height if vertical scrollbar
/// is turned off in order to accommodate viewing the entire contents of the
/// scroll area horizontally or vertically.  The size of the control will grow
/// larger, but will not shrink if the size of the contents widget is reduced.
//=============================================================================
#include "expanding_scroll_area.h"

#include <QScrollBar>
#include <QEvent>

#include "scaling_manager.h"

ExpandingScrollArea::ExpandingScrollArea(QWidget* parent)
    : QScrollArea(parent)
    , size_hint_(QScrollArea::sizeHint())
{
    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ExpandingScrollArea::OnScaleFactorChanged);
}

ExpandingScrollArea::~ExpandingScrollArea()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ExpandingScrollArea::OnScaleFactorChanged);
}

QSize ExpandingScrollArea::sizeHint() const
{
    return size_hint_;
}

QSize ExpandingScrollArea::minimumSizeHint() const
{
    return size_hint_;
}

bool ExpandingScrollArea::eventFilter(QObject* destination_object, QEvent* event)
{
    if ((destination_object != nullptr) && (destination_object == widget()) && (event->type() == QEvent::Resize))
    {
        bool size_updated = false;

        // Expand the width of the scroll area if the horizontal scrollbar is off.
        bool expandable_width = (horizontalScrollBarPolicy() == Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        if (expandable_width == true)
        {
            int new_min_width = widget()->width() + verticalScrollBar()->width();
            if (new_min_width > size_hint_.width())
            {
                size_hint_.setWidth(new_min_width);
                size_updated = true;
            }
        }
        else
        {
            size_hint_.setWidth(QScrollArea::sizeHint().width());
        }

        // Expand the height of the scroll area if the vertical scrollbar is off.
        bool expandable_height = (verticalScrollBarPolicy() == Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        if (expandable_height == true)
        {
            int new_min_height = widget()->height() + horizontalScrollBar()->height();
            if (new_min_height > size_hint_.height())
            {
                size_hint_.setHeight(new_min_height);
                size_updated = true;
            }
        }
        else
        {
            size_hint_.setHeight(QScrollArea::sizeHint().height());
        }

        if (size_updated == true)
        {
            updateGeometry();
            update();
        }
        return true;
    }

    return QScrollArea::eventFilter(destination_object, event);
}

void ExpandingScrollArea::OnScaleFactorChanged()
{
    ResetGeometry();
}

void ExpandingScrollArea::ResetGeometry()
{
    // Adjust the width of the scroll area to match the width of the content if the
    // horizontal scroll bar is off.
    bool expandable_width = (horizontalScrollBarPolicy() == Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    if (expandable_width == true)
    {
        size_hint_.setWidth(widget()->width() + verticalScrollBar()->width());
    }

    // Adjust the height of the scroll area to match the height of the content if the
    // vertical scroll bar is off.
    bool expandable_height = (verticalScrollBarPolicy() == Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    if (expandable_height == true)
    {
        size_hint_.setHeight(widget()->height() + horizontalScrollBar()->height());
    }

    updateGeometry();
    update();
}
