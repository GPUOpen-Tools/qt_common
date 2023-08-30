//=============================================================================
/// Copyright (c) 2016-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a completion bar.
//=============================================================================

#include "completion_bar_widget.h"

#include <QPainter>

#include "scaling_manager.h"

CompletionBarWidget::CompletionBarWidget(QWidget* parent)
    : QWidget(parent)
    , fill_percentage_(0)
{
    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &QWidget::updateGeometry);
}

CompletionBarWidget::~CompletionBarWidget()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &QWidget::updateGeometry);
}

void CompletionBarWidget::SetFillPercentage(qreal percentage)
{
    fill_percentage_ = std::max(0.0, std::min(100.0, percentage));
    emit FillPercentageChanged(percentage);
}

QSize CompletionBarWidget::sizeHint() const
{
    return ScalingManager::Get().Scaled(QSize(kDefaultWidth_, kDefaultHeight_));
}

void CompletionBarWidget::paintEvent(QPaintEvent* paint_event)
{
    Q_UNUSED(paint_event);

    QPainter painter(this);

    QRect rect = this->rect();

    QBrush brush(kEmptyColor_);

    QPen pen(Qt::NoPen);
    painter.setPen(pen);
    painter.fillRect(rect, brush);
    painter.drawRect(rect);

    rect.setRight(rect.right() * (fill_percentage_ / 100.0f));
    brush.setColor(kFillColor_);

    painter.setBrush(brush);
    painter.fillRect(rect, brush);
    painter.drawRect(rect);
}
