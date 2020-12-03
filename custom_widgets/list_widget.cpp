//=============================================================================
/// Copyright (c) 2017-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Implementation of a custom ListWidget that handles scaling and
/// loss of focus
//=============================================================================

#include "list_widget.h"

#include <QApplication>
#include <QCheckBox>
#include <QKeyEvent>
#include <QPainter>
#include <QPushButton>
#include <QScrollBar>

#include "utils/common_definitions.h"
#include "utils/qt_util.h"
#include "utils/scaling_manager.h"

#include "arrow_icon_combo_box.h"
#include "scaled_label.h"
#include "scaled_check_box.h"

ListWidget::ListWidget(QWidget* parent)
    : QListWidget(parent)
    , button_(nullptr)
    , hide_(false)
    , parent_(parent)
    , show_list_above_button_(false)
{
    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ListWidget::OnScaleFactorChanged);
}

ListWidget::ListWidget(QWidget* parent, ArrowIconComboBox* button, bool hide)
    : QListWidget(parent)
    , button_(button)
    , hide_(hide)
    , parent_(parent)
    , show_list_above_button_(false)
{
    connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(FocusChanged(QWidget*, QWidget*)));
    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ListWidget::OnScaleFactorChanged);
}

ListWidget::~ListWidget()
{
    disconnect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(FocusChanged(QWidget*, QWidget*)));
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ListWidget::OnScaleFactorChanged);
}

void ListWidget::OnScaleFactorChanged()
{
    UpdateSize();

    updateGeometry();
    update();
}

void ListWidget::SetListAboveButton(bool list_above_button)
{
    show_list_above_button_ = list_above_button;
}

void ListWidget::FocusChanged(QWidget* old, QWidget* now)
{
    Q_UNUSED(old);
    Q_UNUSED(now);

    if (hide_)
    {
        // Hide the list widget
        hide();

        if (button_ != nullptr)
        {
            // Set the button icon to down arrow
            button_->SetDirection(ArrowIconComboBox::Direction::DownArrow);
        }
    }
}

QSize ListWidget::sizeHint() const
{
    QSize size_hint = QListWidget::sizeHint();

    // Calculate the position
    QRect rect = contentsRect();
    if (button_ != nullptr)
    {
        rect = button_->geometry();
    }

    // Calculate the width and the height of the list widget
    int height = this->GetListWidgetHeight();
    int width  = this->GetListWidgetWidth();

    // If a scrollbar is visible, take the width of it into account.
    // This is necessary so rows aren't clipped under the scrollbar.
    int         scroll_bar_width     = 0;
    QScrollBar* vertical_scroll_bar = verticalScrollBar();
    if (vertical_scroll_bar != nullptr)
    {
        scroll_bar_width = qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
    }

    size_hint.setWidth(width + kDefaultCheckBoxWidth + scroll_bar_width + 2 * frameWidth());

    size_hint.setHeight(height);

    return size_hint;
}

void ListWidget::UpdateSize()
{
    QtCommon::QtUtils::InvalidateFontMetrics(this);

    // update font on each item to match the button font.
    for (int loop = 0; loop < count(); loop++)
    {
        QWidget* item_widget = this->itemWidget(item(loop));
        if (item_widget != nullptr)
        {
            QtCommon::QtUtils::InvalidateFontMetrics(item_widget);
            item_widget->updateGeometry();
        }
    }

    this->updateGeometry();
    this->resize(sizeHint());
}

int ListWidget::GetListWidgetHeight() const
{
    int count  = this->count();
    int height = 0;
    for (int loop = 0; loop < count; loop++)
    {
        height += this->sizeHintForRow(loop);
    }
    height += this->frameWidth() * 2;

    return height;
}

int ListWidget::GetListWidgetWidth() const
{
    int width = 0;
    for (int loop = 0; loop < this->count(); loop++)
    {
        const int item_width = this->sizeHintForColumn(loop);
        if (width < item_width)
        {
            width = item_width;
        }
    }

    return width;
}

void ListWidget::showEvent(QShowEvent* event)
{
    // Update size of the widget before setting position.
    UpdateSize();

    // Recalculate the position so the list widget
    // is shown relative to the parent button.
    RepositionListWidget();

    QWidget::showEvent(event);
}

void ListWidget::paintEvent(QPaintEvent* event)
{
    // Calculate the position when painting just in case the parent pane
    // has scrolled and the widget needs to be moved accordingly.
    RepositionListWidget();

    // Call the base class implementation
    QListWidget::paintEvent(event);
}

void ListWidget::RepositionListWidget()
{
    if (button_ != nullptr)
    {
        QRect  button_rect   = button_->geometry();
        QPoint list_position = button_->mapTo(parent_, QPoint(0, 0));

        list_position.setY(list_position.y() + button_rect.height());

        // if list box is to go above the button, reposition the list
        if (show_list_above_button_ == true)
        {
            list_position.setY(list_position.y() - button_rect.height() - height());
        }

        // Move list to the new position
        this->move(list_position);
    }
}

void ListWidget::AddItem(const QString& label)
{
    QListWidgetItem* item = new QListWidgetItem(this);
    QListWidget::addItem(item);
    ScaledLabel* scaled_label = new ScaledLabel(this);
    scaled_label->setObjectName("scaled_label_" + QString::number(this->count()));
    scaled_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    scaled_label->setText(label);
    scaled_label->setFont(font());
    QListWidget::setItemWidget(item, scaled_label);
}

void ListWidget::SetParent(QWidget* parent)
{
    parent_ = parent;
    setParent(parent);
}

void ListWidget::keyPressEvent(QKeyEvent* event)
{
    Q_ASSERT(button_ != nullptr);

    // If the user pressed the escape key, close the list widget.
    if (event->key() == Qt::Key_Escape)
    {
        emit button_->clicked();
    }
    else
    {
        // For any other key, pass the event onto the base class implementation.
        QListWidget::keyPressEvent(event);
    }
}

void ListWidget::UpdateAllCheckbox(ListWidget*& list_widget)
{
    Q_ASSERT(list_widget != nullptr);

    bool are_all_items_checked = true;

    // Check to see if all of the boxes are checked.
    for (int i = 1; i < list_widget->count(); i++)
    {
        QListWidgetItem* item = list_widget->item(i);
        Q_ASSERT(item != nullptr);

        QCheckBox* check_box = qobject_cast<QCheckBox*>(list_widget->itemWidget(item));
        Q_ASSERT(check_box != nullptr);

        if (check_box->checkState() == Qt::CheckState::Unchecked)
        {
            are_all_items_checked = false;
            break;
        }
    }

    // If update true, all boxes are checked, so update the All check box.
    QListWidgetItem* item = list_widget->item(0);
    if (item != nullptr)
    {
        QCheckBox* check_box = qobject_cast<QCheckBox*>(list_widget->itemWidget(item));
        if (check_box != nullptr)
        {
            Qt::CheckState state = are_all_items_checked ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
            check_box->setCheckState(state);
        }
    }
}

void ListWidget::SetColumnVisibilityCheckboxes(ListWidget*& list_widget, const std::vector<bool>& column_visibility)
{
    Q_ASSERT(list_widget != nullptr);

    // The count of incoming visibility flags must match the number of list items + 1, to account for "All".
    int column_count = list_widget->count();
    Q_ASSERT(column_visibility.size() == (size_t)(column_count - 1));

    // Start at the 1st item instead of the 0th in order to skip the "All" check box item.
    for (int column_index = 1; column_index < column_count; column_index++)
    {
        QListWidgetItem* item = list_widget->item(column_index);
        if (item != nullptr)
        {
            QCheckBox* check_box = qobject_cast<QCheckBox*>(list_widget->itemWidget(item));
            if (check_box != nullptr)
            {
                // Offset the column index to account for the extra "All" item.
                bool is_visible = column_visibility[column_index - 1];
                check_box->setCheckState(is_visible ? Qt::Checked : Qt::Unchecked);
            }
        }
    }

    ListWidget::UpdateAllCheckbox(list_widget);
}

std::vector<bool> ListWidget::GetColumnVisibilityCheckboxes(const ListWidget* list_widget)
{
    std::vector<bool> visibility;

    Q_ASSERT(list_widget != nullptr);
    if (list_widget != nullptr)
    {
        const int column_count = list_widget->count();

        // Subtract 1 to skip over the "All" option.
        visibility.resize(column_count - 1, false);

        // Start from column_index = 1, to skip over the "All" option.
        for (int column_index = 1; column_index < column_count; ++column_index)
        {
            QListWidgetItem* item = list_widget->item(column_index);
            Q_ASSERT(item != nullptr);
            if (item != nullptr)
            {
                QCheckBox* check_box = qobject_cast<QCheckBox*>(list_widget->itemWidget(item));
                if (check_box != nullptr)
                {
                    bool is_visible              = check_box->isChecked();
                    visibility[column_index - 1] = is_visible;
                }
            }
        }
    }

    return visibility;
}

void ListWidget::AddListWidgetCheckboxItem(const QString&            text,
                                           ListWidget*&              list_widget,
                                           std::function<void(bool)> clicked_callback_slot,
                                           QWidget*                  widget,
                                           const QString&            list_name,
                                           const QString&            check_box_name)
{
    Q_ASSERT(list_widget != nullptr);

    // Add each item as a check box.
    QListWidgetItem* item = new QListWidgetItem;
    list_widget->addItem(item);
    QCheckBox* check_box = new ScaledCheckBox(text);
    check_box->setCheckState(Qt::Checked);
    check_box->setObjectName(check_box_name);
    check_box->setFont(list_widget->font());
    list_widget->setItemWidget(item, check_box);

    // Set the mouse cursor to pointing hand cursor.
    check_box->setCursor(Qt::PointingHandCursor);

    // Connect to this check box's stateChanged signal for the appropriate list widget.
    if (list_widget->objectName().compare(list_name) == 0)
    {
        bool is_connected = connect(check_box, &QCheckBox::clicked, widget, clicked_callback_slot);
        Q_ASSERT(is_connected);
        Q_UNUSED(is_connected);
    }
    else
    {
        // Tried to add a check box to the wrong widget.
        Q_ASSERT(false);
    }
}

void ListWidget::focusInEvent(QFocusEvent* event)
{
    emit FocusInEvent();

    // Pass the event onto the base class.
    QListWidget::focusInEvent(event);
}

void ListWidget::focusOutEvent(QFocusEvent* event)
{
    emit FocusOutEvent();

    // Pass the event onto the base class.
    QListWidget::focusOutEvent(event);
}
