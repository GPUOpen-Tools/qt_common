//=============================================================================
/// Copyright (c) 2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Implementation of a QLabel derived widget that truncates the string to
/// the width of the longest widget in the same layout.
//=============================================================================
#include "elided_line_label.h"

#include <QPaintEvent>
#include <QPainter>
#include <QToolTip>

ElidedLineLabel::ElidedLineLabel(QWidget* parent)
    : ScaledLabel(parent)
{
}

ElidedLineLabel::~ElidedLineLabel()
{
}

void ElidedLineLabel::setText(const QString& text_string)
{
    full_text_ = text_string;
    UpdateLabelText();
}

QString ElidedLineLabel::text() const
{
    return full_text_;
}

void ElidedLineLabel::resizeEvent(QResizeEvent* event)
{
    QLabel::resizeEvent(event);
    UpdateLabelText();
}

void ElidedLineLabel::UpdateLabelText()
{
    QString elided_text = fontMetrics().elidedText(full_text_, Qt::ElideRight, width());
    QLabel::setText(elided_text);
    UpdateToolTip();
}

void ElidedLineLabel::UpdateToolTip()
{
    // Only set the tooltip if the label text is truncated.
    if (QLabel::text() != full_text_)
    {
        // Force the tooltip to be rich text by adding the <font> tag.
        // This allows the tooltip rendering to word wrap the text.
        QString rich_text = "<font>" + full_text_ + "</font>";
        setToolTip(rich_text);
    }
    else
    {
        setToolTip("");
    }
}
