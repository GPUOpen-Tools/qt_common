//=============================================================================
/// Copyright (c) 2019-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for ScaledCheckBox objects.
//=============================================================================
#include "scaled_check_box.h"

#include <QStyle>
#include <QEvent>

#include "utils/qt_util.h"
#include "utils/scaling_manager.h"

ScaledCheckBox::ScaledCheckBox(QWidget* parent)
    : QCheckBox(parent)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledCheckBox::OnScaleFactorChanged);
}

ScaledCheckBox::ScaledCheckBox(const QString& text, QWidget* parent)
    : QCheckBox(text, parent)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledCheckBox::OnScaleFactorChanged);
}

ScaledCheckBox::~ScaledCheckBox()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledCheckBox::OnScaleFactorChanged);
}

QSize ScaledCheckBox::sizeHint() const
{
    ensurePolished();

    int icon_width  = this->style()->pixelMetric(QStyle::PM_IndicatorWidth, nullptr, this);
    int icon_height = this->style()->pixelMetric(QStyle::PM_IndicatorHeight, nullptr, this);
    int spacing     = this->style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, nullptr, this);

    // Calculate length of the text string,
    // using TextShowMnemonic so that keyboard shortcuts indicated by '&' are taken into account.
    const QRect& rect       = fontMetrics().boundingRect(text());
    int          text_width = fontMetrics().boundingRect(rect, Qt::TextShowMnemonic, text()).width();

    QSize size;
    size.setWidth(icon_width + spacing + text_width);
    size.setHeight(std::max(icon_height, fontMetrics().height()));

    return size;
}

void ScaledCheckBox::UpdateIndicatorSize()
{
    // It appears the only way (or at least easiest way) to set the indicator size is through the stylesheet.
    this->setStyleSheet(kIndicatorSizeStylesheet_.arg(fontMetrics().ascent()));
}

void ScaledCheckBox::OnScaleFactorChanged()
{
    // Invalidating the font metrics will ensure the metrics match the font size.
    // This will also trigger a fontChanged event, which is overridden in this class
    // to update the size of the indicator.
    QtCommon::QtUtils::InvalidateFontMetrics(this);

    // Update the indicator size to match the fonts.
    UpdateIndicatorSize();

    updateGeometry();
    update();
}

void ScaledCheckBox::changeEvent(QEvent* event)
{
    QCheckBox::changeEvent(event);

    if (!invalidating_font_metrics_)
    {
        if (event->type() == QEvent::FontChange)
        {
            invalidating_font_metrics_ = true;
            QtCommon::QtUtils::InvalidateFontMetrics(this);
            invalidating_font_metrics_ = false;

            UpdateIndicatorSize();

            updateGeometry();
        }
    }
}
