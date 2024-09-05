//=============================================================================
// Copyright (c) 2024 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief  Implementation of the custom banner widget.
//=============================================================================

#include "banner_widget.h"
#include "ui_banner_widget.h"

#include <QPainter>

// Static strings used for label widgets.
static const QString kBannerText        = "[Notification message]";
static const QString kCloseLink         = "Close";
static const QString kCloseText         = QChar(0x2A2F);
static const QString kShowDetailsText   = "See details";
static const QString kShowDetailsLink   = "ShowDetails";
static const QString kDontShowAgainText = "Do not show again";
static const QString kDontShowAgainLink = "DontShowAgain";
static const QString kStyledButtonHtml  = "<a style=\"text-decoration:none; color:black; font-size:xx-large; font-weight:600;\" href=\"%1\">%2</a>";
static const QString kCloseToolTip      = "Close the notification banner.";

BannerWidget::BannerWidget(QWidget* parent)
    : QWidget(parent)
    , ui_(new Ui::BannerWidget)
{
    ui_->setupUi(this);
    Init();
}

BannerWidget::~BannerWidget()
{
}

void BannerWidget::Init()
{
    SetNotificationText(kBannerText);
    ui_->show_details_label_->SetLink(kShowDetailsText, kShowDetailsLink);
    ui_->dont_show_again_label_->SetLink(kDontShowAgainText, kDontShowAgainLink);
    ui_->close_button_->setText(kStyledButtonHtml.arg(kCloseLink, kCloseText));
    ui_->close_button_->setWindowTitle(kCloseToolTip);

    connect(ui_->show_details_label_, &QLabel::linkActivated, this, &BannerWidget::HandleShowDetailsClicked);
    connect(ui_->dont_show_again_label_, &QLabel::linkActivated, this, &BannerWidget::HandleDontShowAgainClicked);
    connect(ui_->close_button_, &QLabel::linkActivated, this, &BannerWidget::HandleCloseClicked);

    // Install an event filter to handle propagating text color to the child labels.
    installEventFilter(this);

    // Hide the banner until it is needed.
    hide();
}

void BannerWidget::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);

    // Draw background for the button.
    painter.setBrush(QBrush(palette().color(QPalette::Window)));
    painter.drawRect(0, 0, width(), height());
}

bool BannerWidget::eventFilter(QObject* object, QEvent* event)
{
    const bool return_value = QWidget::eventFilter(object, event);

    if ((event->type() == QEvent::PaletteChange) || (event->type() == QEvent::Polish))
    {
        // Propagate the text color to the child label widgets.
        const QColor text_color = palette().color(QPalette::WindowText);

        // Iterate over all child widgets and set the text color for labels.
        for (QObject* child : children())
        {
            QLabel* label = qobject_cast<QLabel*>(child);
            if (label != nullptr)
            {
                // Get the label's palette and set the text color.
                QPalette palette = label->palette();
                palette.setColor(QPalette::WindowText, text_color);
                label->setPalette(palette);
            }
        }
    }

    return return_value;
}

void BannerWidget::HandleShowDetailsClicked(const QString& link)
{
    if (link == kShowDetailsLink)
    {
        emit ShowDetailsClicked();
    }
}

void BannerWidget::SetNotificationText(const QString& notification_text)
{
    ui_->message_label_->setText(notification_text);
}

void BannerWidget::HandleDontShowAgainClicked(const QString& link)
{
    if (link == kDontShowAgainLink)
    {
        DontShowAgainQuery();
    }
}

void BannerWidget::HandleCloseClicked(const QString& link)
{
    if (link == kCloseLink)
    {
        hide();
        emit CloseClicked();
    }
}

void BannerWidget::DontShowAgainQuery()
{
    emit DontShowAgainRequested();
}

void BannerWidget::SetLinkColor(const QColor& color)
{
    ui_->show_details_label_->SetLinkColor(color);
    ui_->dont_show_again_label_->SetLinkColor(color);
}

void BannerWidget::SetDisabledLinkColor(const QColor& color)
{
    ui_->show_details_label_->SetDisabledLinkColor(color);
    ui_->dont_show_again_label_->SetDisabledLinkColor(color);
}
