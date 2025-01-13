//=============================================================================
// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief  Implementation of the custom tree view widget for driver overrides.
//=============================================================================

#include "driver_overrides_tree_widget.h"
#include "ui_driver_overrides_tree_widget.h"

#include "driver_overrides_model.h"
#include "scaled_label.h"
#include "scaled_tree_view.h"

#include <QFontMetrics>
#include <QHBoxLayout>
#include <QHelpEvent>
#include <QLabel>
#include <QToolTip>

DriverOverridesTreeWidget::DriverOverridesTreeWidget(QWidget* parent)
    : QWidget(parent)
    , ui_(new Ui::DriverOverridesTreeWidget)
{
    ui_->setupUi(this);
    Init();
}

DriverOverridesTreeWidget::~DriverOverridesTreeWidget()
{
}

void DriverOverridesTreeWidget::Init()
{
    // Set up the model for the widgets.
    driver_overrides::DriverOverridesModel* driver_overrides_model = driver_overrides::DriverOverridesModel::GetInstance();
    ui_->tree_view_->setModel(driver_overrides_model);

    // Set the root index for the tree view to the root of the driver overrides tree in the model.
    // This allows the tree view to display only the driver overrides and not other properties in the model.
    ui_->tree_view_->setRootIndex(driver_overrides_model->GetDriverOverridesSubTreeIndex());

    // Map the model's properties to the widgets.
    driver_overrides_model->BindWidgetToModelAttribute(
        driver_overrides::kModelAttributeNameTreeViewTitle, ui_->title_label_, "text", &label_title_text_mapper_);
    driver_overrides_model->BindWidgetToModelAttribute(driver_overrides::kModelAttributeNameDriverOverridesPresent, this, "visible", &tree_view_mapper_);

    // Override the Scaled Tree View's default settings.
    ScaledTreeView* tree_view = ui_->tree_view_;
    tree_view->setAlternatingRowColors(false);
    tree_view->setSelectionMode(QAbstractItemView::NoSelection);
    tree_view->header()->setStretchLastSection(false);

    // If the placeholder widget (i.e. DriverOverridesTreeWidget) has a verical size policy of minimum, this is used to indicate that
    // the tree view is contained within a scrollarea and should be allowed to expand to its full height.  In this case, the tree view's
    // virtual size policy should be set to minimum and the horizontal size policy set to preferred to limit the horizontal size rather
    // than allowing it to strched to the full window width.
    if (sizePolicy().verticalPolicy() == QSizePolicy::Minimum)
    {
        tree_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    }

    // Add an event filter to the tree view to handle displaying tooltips only when the mouse is over text.
    tree_view->installEventFilter(this);

    // Add an event filter to the title label so that font changes can be tracked.
    // The font used by the model for the tree view is updated to match the title label's font.
    ui_->title_label_->installEventFilter(this);

    // Connect the slot to handle expanding the tree view when driver overrides are imported.
    connect(driver_overrides_model, &driver_overrides::DriverOverridesModel::DriverOverridesImported, this, &DriverOverridesTreeWidget::UpdateView);
}

void DriverOverridesTreeWidget::UpdateView()
{
    // Make sure all child items are expanded in the tree view.
    ui_->tree_view_->expandRecursively(ui_->tree_view_->rootIndex());
}

// Make the tooltip only display when the mouse is hovering over text for an item and not the entire row.
bool DriverOverridesTreeWidget::eventFilter(QObject* object, QEvent* event)
{
    if (object == ui_->tree_view_)
    {
        if (event->type() == QEvent::ToolTip)
        {
            bool              tooltip_shown = false;
            const QHelpEvent* help_event    = static_cast<QHelpEvent*>(event);
            const QModelIndex index         = ui_->tree_view_->indexAt(help_event->pos());
            if (index.isValid())
            {
                // Get the text for the item.
                const QString item_text = index.data(Qt::DisplayRole).toString();
                if (!item_text.isEmpty())
                {
                    // Get the length in pixels of the text.
                    QFontMetrics metrics(ui_->tree_view_->font());
                    QRect        text_bounding_rect = metrics.boundingRect(item_text);

                    // Convert the global position of the mouse to the local position of the tree view item.
                    const QPoint local_pos = ui_->tree_view_->viewport()->mapFromGlobal(help_event->globalPos());

                    // Adjust the bounding rectangle to the tree view item.
                    text_bounding_rect.moveTo(ui_->tree_view_->visualRect(index).topLeft());

                    // If the mouse is over the text, show the tooltip.
                    if (text_bounding_rect.contains(local_pos))
                    {
                        const QString tooltip_text = index.data(driver_overrides::DriverOverridesModel::custom_tooltip_role_).toString();
                        QToolTip::showText(help_event->globalPos(), tooltip_text);
                        tooltip_shown = true;
                    }
                }
            }

            if (!tooltip_shown)
            {
                QToolTip::hideText();
                event->ignore();
            }
        }
    }
    else if (object == ui_->title_label_)
    {
        if (event->type() == QEvent::FontChange)
        {
            // Process the event so the label's font is updated.
            QWidget::eventFilter(object, event);

            // Make the model's font match the title label's font.
            driver_overrides::DriverOverridesModel::GetInstance()->SetDefaultItemFont(ui_->title_label_->font());
            event->ignore();
        }
    }

    return QWidget::eventFilter(object, event);
}
