//=============================================================================
/// Copyright (c) 2017-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a QPushButton with three lines of text. The first
/// line is a title which is drawn in bold, and the second two lines are
/// description, drawn using the same font but not in bold. The text changes
/// color when the mouse is over the button.
//=============================================================================

#include "quick_link_button_widget.h"

#include <QPainter>
#include <QStyleOptionButton>
#include <QStylePainter>

#include "common_definitions.h"
#include "qt_util.h"
#include "scaling_manager.h"

QuickLinkButtonWidget::QuickLinkButtonWidget(QWidget* parent)
    : QPushButton("test", parent)
    , title_string_("")
    , description_line_one_("")
    , description_line_two_("")
{
    setCursor(Qt::PointingHandCursor);
    setContentsMargins(kContentsMargin_, kContentsMargin_, kContentsMargin_, kContentsMargin_);

    QFont tmp_font = font();
    tmp_font.setPointSizeF(kButtonFontPointSize);
    setFont(tmp_font);

    highlighted_ = false;

    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &QuickLinkButtonWidget::OnScaleFactorChanged);
}

QuickLinkButtonWidget::~QuickLinkButtonWidget()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &QuickLinkButtonWidget::OnScaleFactorChanged);
}

void QuickLinkButtonWidget::OnScaleFactorChanged()
{
    QtCommon::QtUtils::InvalidateFontMetrics(this);
    updateGeometry();
}

QSize QuickLinkButtonWidget::sizeHint() const
{
    // Setup a temporary font to get fontMetrics for bold and non-bold fonts.
    QFont tmp_font(font());
    tmp_font.setBold(true);
    QFontMetrics title_font_metrics(tmp_font);

    tmp_font.setBold(false);
    QFontMetrics desc_font_metrics(tmp_font);

    // The width is defined by the width of the widest line of text, based on the font they will be drawn with.
    int width = std::max(std::max(title_font_metrics.horizontalAdvance(title_string_), desc_font_metrics.horizontalAdvance(description_line_one_)),
                         desc_font_metrics.boundingRect(description_line_two_).width());
    width += (contentsMargins().left() + contentsMargins().right());

    // The height is defined by the height of the title plus two lines of description plus two line spacing between the text.
    // Then add two more margins for above the title and below the second line of description.
    int height = title_font_metrics.height() + 2 * desc_font_metrics.height() + 2 * kLineSpacing_;
    height += (contentsMargins().top() + contentsMargins().bottom());

    return QSize(width, height);
}

void QuickLinkButtonWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    // Setup fonts that will be used.
    QFont title_font(painter.font());
    title_font.setBold(true);

    QFont desc_font(painter.font());
    desc_font.setBold(false);

    // Draw background for the button.
    painter.setPen(QtCommon::QtUtils::ColorTheme::Get().GetCurrentThemeColors().quick_link_button_background_color);
    painter.setBrush(QtCommon::QtUtils::ColorTheme::Get().GetCurrentThemeColors().quick_link_button_background_color);

    painter.drawRect(0, 0, width(), height());

    // Initialize X and Y offsets to be in the top left corner of the contents rect.
    const int x_offset = contentsRect().left();
    int       y_offset = contentsRect().top();

    // Set the font so that the metrics are measured properly.
    // Update the y_offset by the font ascent so that it points to the desired
    // baseline of the title text.
    painter.setFont(title_font);

    if (highlighted_)
    {
        painter.setPen(QtCommon::QtUtils::ColorTheme::Get().GetCurrentThemeColors().link_text_color);
    }
    else
    {
        painter.setPen(QtCommon::QtUtils::ColorTheme::Get().GetCurrentThemeColors().graphics_scene_text_color);
    }

    y_offset += painter.fontMetrics().ascent();
    painter.drawText(x_offset, y_offset, title_string_);

    // Move down additional line spacing, and below the first line of description text.
    painter.setFont(desc_font);
    y_offset += kLineSpacing_ + painter.fontMetrics().height();
    painter.drawText(x_offset, y_offset, description_line_one_);

    // Move down additional line spacing, and below the second line of description text.
    y_offset += kLineSpacing_ + painter.fontMetrics().height();
    painter.drawText(x_offset, y_offset, description_line_two_);
}

void QuickLinkButtonWidget::leaveEvent(QEvent* event)
{
    QPushButton::leaveEvent(event);

    highlighted_ = false;
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void QuickLinkButtonWidget::enterEvent(QEvent* event)
#else
void QuickLinkButtonWidget::enterEvent(QEnterEvent* event)
#endif
{
    QPushButton::enterEvent(event);

    highlighted_ = true;
}
