//=============================================================================
/// Copyright (c) 2016-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a check box widget.
//=============================================================================

#include "check_box_widget.h"

#include <QStyle>

#include "common_definitions.h"
#include "qt_util.h"
#include "scaling_manager.h"

static const QString kStringOff = "OFF";  ///< Default off text for the checkbox.
static const QString kStringOn  = "ON";   ///< Default on text for the checkbox.

CheckBoxWidget::CheckBoxWidget(QWidget* parent)
    : QCheckBox(parent)
{
    on_text_  = kStringOn;
    off_text_ = kStringOff;
    setText(off_text_);

    // Update the indicator size to match the default font size.
    UpdateIndicatorSize();

    connect(this, &QCheckBox::stateChanged, this, &CheckBoxWidget::StateChanged);
    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &CheckBoxWidget::OnScaleFactorChanged);
}

CheckBoxWidget::~CheckBoxWidget()
{
    disconnect(this, &QCheckBox::stateChanged, this, &CheckBoxWidget::StateChanged);
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &CheckBoxWidget::OnScaleFactorChanged);
}

void CheckBoxWidget::OnScaleFactorChanged()
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

void CheckBoxWidget::SetOnText(const QString& text)
{
    on_text_ = text;

    if (isChecked())
    {
        QCheckBox::setText(text);
    }
}

void CheckBoxWidget::SetOffText(const QString& text)
{
    off_text_ = text;

    if (!isChecked())
    {
        QCheckBox::setText(text);
    }
}

void CheckBoxWidget::setText(const QString& text)
{
    if (text != on_text_ && text != off_text_)
    {
        StateChanged(checkState());
    }
    else
    {
        QCheckBox::setText(text);
    }
}

QSize CheckBoxWidget::sizeHint() const
{
    ensurePolished();

    int icon_width  = this->style()->pixelMetric(QStyle::PM_IndicatorWidth, nullptr, this);
    int icon_height = this->style()->pixelMetric(QStyle::PM_IndicatorHeight, nullptr, this);
    int spacing     = this->style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, nullptr, this);

    // Calculate max length of the on and off text strings,
    // using TextShowMnemonic so that keyboard shortcuts indicated by '&' are taken into account.
    const QRect& on_rect  = fontMetrics().boundingRect(on_text_);
    const QRect& off_rect = fontMetrics().boundingRect(off_text_);

    int on_text_width  = fontMetrics().boundingRect(on_rect, Qt::TextShowMnemonic, on_text_).width();
    int off_text_width = fontMetrics().boundingRect(off_rect, Qt::TextShowMnemonic, off_text_).width();
    int max_text_width = std::max(on_text_width, off_text_width);

    QSize size;
    size.setWidth(icon_width + spacing + max_text_width);
    size.setHeight(std::max(icon_height, fontMetrics().height()));

    return size;
}

void CheckBoxWidget::UpdateIndicatorSize()
{
    // It appears the only way (or at least easiest way) to set the indicator size is through the stylesheet.
    this->setStyleSheet(kIndicatorSizeStylesheet.arg(fontMetrics().height() * 2).arg(fontMetrics().height() * 1));
}

void CheckBoxWidget::StateChanged(int checkbox_state)
{
    if (checkbox_state == Qt::Checked)
    {
        setText(on_text_);
    }
    else if (checkbox_state == Qt::Unchecked)
    {
        setText(off_text_);
    }
    else
    {
        Q_ASSERT("Unsupported CheckBox state");
    }
}
