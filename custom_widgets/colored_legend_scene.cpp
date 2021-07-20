//=============================================================================
/// Copyright (c) 2016-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation for a class that contains colored legends
//=============================================================================

#include "colored_legend_scene.h"

#include <QDebug>
#include <QGraphicsRectItem>

#include "utils/scaling_manager.h"
#include "utils/qt_util.h"

#include "colored_legend_graphics_view.h"

ColoredLegendScene::ColoredLegendScene(QWidget* parent)
    : QGraphicsScene(parent)
    , legend_mode_(LegendMode::kColor)
{
    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ColoredLegendScene::Update);
}

ColoredLegendScene::~ColoredLegendScene()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ColoredLegendScene::Update);
}

void ColoredLegendScene::AddColorLegendItem(const QColor& color, const QString& description)
{
    legend_mode_ = LegendMode::kColor;

    QFont        text_font = this->font();
    QFontMetrics font_metrics(text_font);
    if (this->views().count() > 0)
    {
        QtCommon::QtUtils::InvalidateFontMetrics(this->views()[0]);
        font_metrics   = this->views()[0]->fontMetrics();
        text_font = this->views()[0]->font();
    }

    // Set size of square based on font height + offsets above and below.
    qreal square_height = font_metrics.height() + (2 * kVerticalSpacingAroundText);

    ColorLegendItem legend_item = ColorLegendItem();
    legend_item.rect_item_      = new QGraphicsRectItem();
    legend_item.text_item_      = new QGraphicsTextItem(description);

    legend_item.text_item_->setPos(0, 0);
    legend_item.text_item_->setFont(text_font);

    legend_item.rect_item_->setRect(0, 0, square_height, square_height);
    legend_item.rect_item_->setBrush(color);
    legend_item.rect_item_->setPen(color);

    color_legends_.push_back(legend_item);

    // addItem(QGraphicsItem*) takes ownership of the item,
    // so we do not need to delete it before deleting the scene.
    addItem(legend_item.rect_item_);
    addItem(legend_item.text_item_);

    // Update the scene to account for DPI scaling and to cause the view to resize.
    Update();
}

void ColoredLegendScene::AddTextLegendItem(const QString& description)
{
    legend_mode_ = LegendMode::kText;

    QGraphicsTextItem* text_item = new QGraphicsTextItem(description);
    text_item->setPos(0, 0);

    text_legends_.push_back(text_item);

    // addItem(QGraphicsItem*) takes ownership of the item,
    // so we do not need to delete it before deleting the scene.
    addItem(text_item);

    // Update the scene to account for DPI scaling and to cause the view to resize.
    Update();
}

void ColoredLegendScene::Clear()
{
    for (int i = 0; i < color_legends_.size(); i++)
    {
        removeItem(color_legends_[i].rect_item_);
        removeItem(color_legends_[i].text_item_);
        delete color_legends_[i].rect_item_;
        delete color_legends_[i].text_item_;
    }

    color_legends_.clear();

    for (int i = 0; i < text_legends_.size(); i++)
    {
        removeItem(text_legends_[i]);
        delete text_legends_[i];
    }

    text_legends_.clear();
}

void ColoredLegendScene::Update()
{
    QFont        text_font = this->font();
    QFontMetrics font_metrics(text_font);

    // Take the font metrics from the view if available.
    if (this->views().count() > 0)
    {
        QtCommon::QtUtils::InvalidateFontMetrics(views()[0]);
        font_metrics = views()[0]->fontMetrics();
        text_font    = views()[0]->font();
    }

    // Block size is calculated based on the height of the text,
    // plus a little extra above and below the text.
    const qreal scaled_base_height = font_metrics.height() + (2 * kVerticalSpacingAroundText);

    const qreal y_pos_top = 0;
    qreal       x_pos     = 0;

    if (legend_mode_ == LegendMode::kColor)
    {
        for (int i = 0; i < color_legends_.size(); i++)
        {
            const ColorLegendItem& item = color_legends_[i];

            item.rect_item_->setPos(x_pos, y_pos_top);
            item.rect_item_->setRect(0, 0, scaled_base_height, scaled_base_height);
            item.text_item_->setPos(x_pos + scaled_base_height, y_pos_top - kVerticalSpacingAroundText);
            item.text_item_->setFont(text_font);
            const int text_width = font_metrics.boundingRect(item.text_item_->toPlainText()).width();

            x_pos += scaled_base_height + text_width + kHorizontalSpacingAfterText;
        }
    }
    else if (legend_mode_ == LegendMode::kText)
    {
        for (int i = 0; i < text_legends_.size(); i++)
        {
            text_legends_[i]->setPos(x_pos, y_pos_top);
            text_legends_[i]->setFont(text_font);
            const int text_width = font_metrics.boundingRect(text_legends_[i]->toPlainText()).width();

            x_pos += text_width + ScalingManager::Get().Scaled(kHorizontalSpacingAfterText);
        }
    }

    // Make sure associated views are updated to the new size.
    for (QGraphicsView* view : views())
    {
        ColoredLegendGraphicsView* legend_view = qobject_cast<ColoredLegendGraphicsView*>(view);
        if (legend_view != nullptr)
        {
            legend_view->Update();
        }
    }
}
