//=============================================================================
/// Copyright (c) 2024 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Implementation for QLabel derived object that contains a hyperlink.
//=============================================================================
#include "scaled_link_label.h"

#include <QEvent>

#include "common_definitions.h"
#include "qt_util.h"
#include "scaling_manager.h"

static const QString kStyledLinkHtml = "<a style=\" color: %1;\" href=\"%2\">%3</a>";

ScaledLinkLabel::ScaledLinkLabel(QWidget* parent)
    : ScaledLabel(parent)
{
    installEventFilter(this);
}

ScaledLinkLabel::~ScaledLinkLabel()
{
}

const QString& ScaledLinkLabel::GetLinkText() const
{
    return link_text_;
}

const QString& ScaledLinkLabel::GetLinkUrl() const
{
    return link_url_;
}

void ScaledLinkLabel::SetLink(const QString& text, const QString& url)
{
    link_text_ = text;
    link_url_  = url;
    UpdateLinkColor();
}

const QColor& ScaledLinkLabel::GetLinkColor() const
{
    return link_color_;
}

void ScaledLinkLabel::SetLinkColor(const QColor& color)
{
    link_color_ = color;
    UpdateLinkColor();
}

const QColor& ScaledLinkLabel::GetDisabledLinkColor() const
{
    return disabled_link_color_;
}

void ScaledLinkLabel::SetDisabledLinkColor(const QColor& color)
{
    disabled_link_color_ = color;
    UpdateLinkColor();
}

void ScaledLinkLabel::UpdateLinkColor()
{
    QString color;
    if (isEnabled())
    {
        color = link_color_.name();
    }
    else
    {
        color = disabled_link_color_.name();
    }
    setText(kStyledLinkHtml.arg(color, link_url_, link_text_));
}

bool ScaledLinkLabel::eventFilter(QObject* object, QEvent* event)
{
    const bool return_value = ScaledLabel::eventFilter(object, event);

    if (object == this)
    {
        if ((event->type() == QEvent::EnabledChange) || (event->type() == QEvent::PaletteChange) || (event->type() == QEvent::Polish))
        {
            UpdateLinkColor();

            // Set the mouse cursor if the link is enabled.
            if (isEnabled())
            {
                setCursor(Qt::PointingHandCursor);
            }
            else
            {
                setCursor(Qt::ArrowCursor);
            }
        }
    }

    return return_value;
}
