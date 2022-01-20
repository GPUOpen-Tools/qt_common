//=============================================================================
/// Copyright (c) 2018-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of our down arrow combobox
//=============================================================================

#include "arrow_icon_combo_box.h"

#include <QApplication>
#include <QCheckBox>
#include <QPainter>

#include "utils/scaling_manager.h"
#include "utils/qt_util.h"

#include "scaled_check_box.h"
#include "scaled_label.h"

const static int kAllIndex       = 0;  ///< The index of the "All" checkbox, if used.
static const int kTextOffsetX    = 4;  ///< The spacing between the arrow and text, and after the text.
static const int kButtonBaseSize = 18; ///< The dimensions of the arrow when viewed at 100% DPI.
static const int kPenWidth       = 3;  ///< The width of the pen used to draw the arrow.

ArrowIconComboBox::ArrowIconComboBox(QWidget* parent)
    : QPushButton(parent)
    , allow_multi_select_(false)
    , show_list_above_button_(false)
    , retain_default_text_(false)
    , all_choice_added_(false)
    , all_choice_selected_(false)
    , size_(kButtonBaseSize)
    , color_(Qt::GlobalColor::gray)
    , border_color_(Qt::GlobalColor::black)
    , show_border_(false)
    , font_color_(Qt::GlobalColor::black)
    , direction_(Direction::DownArrow)
    , pen_width_(kPenWidth)
    , highlight_substring_(false)
{
    setCursor(Qt::PointingHandCursor);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    CreateVertices();

    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ArrowIconComboBox::OnScaleFactorChanged);
}

ArrowIconComboBox::~ArrowIconComboBox()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ArrowIconComboBox::OnScaleFactorChanged);

    if (item_list_.get() != nullptr)
    {
        disconnect(item_list_.get(), &ListWidget::itemClicked, this, &ArrowIconComboBox::ListItemClicked);
    }

    DisconnectSignals();
}

void ArrowIconComboBox::OnScaleFactorChanged()
{
    QtCommon::QtUtils::InvalidateFontMetrics(this);

    // Setting a new scaled size will cause the vertices to get recalculated
    // and update the geometry of the widget.
    SetSize(ScalingManager::Get().Scaled(kButtonBaseSize));

    updateGeometry();
    update();
}

void ArrowIconComboBox::InitItemList(QWidget* main_parent)
{
    // Create list widget for the combo box
    item_list_ = std::unique_ptr<ListWidget>(new ListWidget(main_parent, this, false));
    item_list_->setObjectName(this->objectName() + "_item_list");

    // Also disable scrollbars on this list widget
    item_list_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    item_list_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    item_list_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    item_list_->setFont(font());

    // The item list should not be visible by default.
    item_list_->hide();
}

void ArrowIconComboBox::InitSingleSelect(QWidget* main_parent, const QString& default_text, bool retain_default_text, const QString& prefix_text)
{
    InitItemList(main_parent);
    allow_multi_select_  = false;
    default_text_        = default_text;
    retain_default_text_ = retain_default_text;

    if (retain_default_text == false)
    {
        prefix_text_ = prefix_text;
    }

    // Start out the combo box with the first entry
    item_list_->setCurrentRow(0);
    item_list_->hide();

    ResetSelection();

    item_list_->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

    connect(item_list_.get(), &ListWidget::itemClicked, this, &ArrowIconComboBox::ListItemClicked);

    qApp->installEventFilter(this);
}

void ArrowIconComboBox::InitMultiSelect(QWidget* main_parent, const QString& default_text)
{
    InitItemList(main_parent);
    allow_multi_select_ = true;
    default_text_       = default_text;

    // Since the list is multi-select, and we don't currently have a need or way to combine all the selected items,
    // automatically opt to retain the default text that was set for this combobox.
    retain_default_text_ = true;

    ResetSelection();

    // The multi-select combobox will use checkboxes instead of labels, and the state of those checkboxes
    // will override the selected state of the list items.
    item_list_->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);

    connect(item_list_.get(), &ListWidget::itemClicked, this, &ArrowIconComboBox::ListItemClicked);

    qApp->installEventFilter(this);
}

void ArrowIconComboBox::SetListAboveButton(bool list_above_button)
{
    SetDirection(ArrowIconComboBox::Direction::UpArrow);
    item_list_->SetListAboveButton(list_above_button);
    show_list_above_button_ = list_above_button;
}

void ArrowIconComboBox::Update()
{
    if (item_list_ != nullptr && item_list_->count() == 0)
    {
        CloseMenu();

        ResetSelection();
    }
}

void ArrowIconComboBox::ResetSelection()
{
    setText(default_text_);
}

int ArrowIconComboBox::PrevRow() const
{
    // the item list must not be null
    Q_ASSERT(item_list_ != nullptr);

    if (item_list_ != nullptr)
    {
        const int current_row  = item_list_->currentRow();
        const int previous_row = current_row - 1;
        if (previous_row < 0)
        {
            return item_list_->currentRow();
        }
        else
        {
            return previous_row;
        }
    }

    return -1;
}

int ArrowIconComboBox::CurrentRow() const
{
    // the item list must not be null
    Q_ASSERT(item_list_ != nullptr);

    if (item_list_ != nullptr)
    {
        return item_list_->currentRow();
    }

    return -1;
}

int ArrowIconComboBox::NextRow() const
{
    // the item list must not be null
    Q_ASSERT(item_list_ != nullptr);

    if (item_list_ != nullptr)
    {
        const int current_row = item_list_->currentRow();
        const int next_row    = current_row + 1;
        if (next_row >= item_list_->count())
        {
            return item_list_->currentRow();
        }
        else
        {
            return next_row;
        }
    }

    return -1;
}

int ArrowIconComboBox::RowCount() const
{
    if (item_list_ != nullptr)
    {
        return item_list_->count();
    }
    else
    {
        return 0;
    }
}

void ArrowIconComboBox::SetSelectedRow(int index)
{
    // the item list must not be null
    Q_ASSERT(item_list_ != nullptr);

    if (item_list_ != nullptr)
    {
        if (index >= 0 && index < item_list_->count())
        {
            QListWidgetItem* item = item_list_->item(index);

            if (item != nullptr)
            {
                // Set the current row in the list.
                item_list_->setCurrentRow(index);

                // Set the combobox text.
                QWidget* widget = item_list_->itemWidget(item);
                if (widget == nullptr)
                {
                    // No itemWidget is being used, so get the text directly from the item.
                    // If this codepath gets used, then this item probably has DPI scaling issues,
                    // and a Scaled* widget should be used instead when the item is added to the combobox.
                    SetSelectedText(item->text());
                }
                else
                {
                    ScaledLabel* label = dynamic_cast<ScaledLabel*>(widget);
                    if (label != nullptr)
                    {
                        SetSelectedText(label->text());
                    }
                    else
                    {
                        ScaledCheckBox* checkbox = dynamic_cast<ScaledCheckBox*>(widget);
                        if (checkbox != nullptr)
                        {
                            SetSelectedText(checkbox->text());
                        }
                    }
                }
            }
        }
    }
}

void ArrowIconComboBox::AllCheckboxClicked(bool checked)
{
    QListWidgetItem* current_item = nullptr;
    for (int i = kAllIndex + 1; i < item_list_->count(); i++)
    {
        current_item        = item_list_->item(i);
        QCheckBox* checkbox = qobject_cast<QCheckBox*>(item_list_->itemWidget(current_item));
        if (checkbox != nullptr)
        {
            checkbox->setChecked(checked);
        }
    }
}

void ArrowIconComboBox::UpdateAllOption(bool recently_checked)
{
    if (all_choice_added_)
    {
        // assume the "All" button should be the same state as the checkbox selected
        bool all_items_checked = recently_checked;
        if (recently_checked == true)
        {
            // if a checkbox is checked, the "All" checkbox needs checking iff all other checkboxes are checked
            for (int i = kAllIndex + 1; i < item_list_->count(); i++)
            {
                if (IsChecked(i) == false)
                {
                    all_items_checked = false;
                    break;
                }
            }
        }

        // set the state of the "All" checkbox
        QListWidgetItem* all_item     = item_list_->item(kAllIndex);
        QCheckBox*       all_checkbox = qobject_cast<QCheckBox*>(item_list_->itemWidget(all_item));
        if (all_checkbox != nullptr)
        {
            all_checkbox->setChecked(all_items_checked);
        }
    }
}

void ArrowIconComboBox::OnCheckboxClicked(bool checked)
{
    UpdateAllOption(checked);

    emit CheckboxStateChanged(checked);

    QCheckBox* checkbox = qobject_cast<QCheckBox*>(sender());
    Q_ASSERT(checkbox != nullptr);
    if (checkbox != nullptr)
    {
        emit CheckboxChanged(checkbox);
    }
}

void ArrowIconComboBox::SetRowHidden(int list_index, bool hidden)
{
    Q_ASSERT(item_list_ != nullptr);
    Q_ASSERT(list_index >= 0 && list_index < item_list_->count());

    item_list_->setRowHidden(list_index, hidden);
}

void ArrowIconComboBox::SetMaximumHeight(int height)
{
    if (item_list_ != nullptr)
    {
        item_list_->setMaximumHeight(height);
    }
}

void ArrowIconComboBox::SetFixedWidth(int width)
{
    setFixedWidth(width);

    if (item_list_ != nullptr)
    {
        item_list_->setFixedWidth(width);
    }
}

void ArrowIconComboBox::ClearItems()
{
    all_choice_added_    = false;
    all_choice_selected_ = false;
    if (item_list_ != nullptr)
    {
        item_list_->clear();
    }
}

void ArrowIconComboBox::AddItem(QListWidgetItem* item)
{
    Q_ASSERT(item_list_ != nullptr);
    if (item_list_ != nullptr)
    {
        Q_ASSERT(!allow_multi_select_);
        item_list_->addItem(item);
    }
}

void ArrowIconComboBox::AddItem(const QString& item_text, const QVariant& user_data)
{
    Q_ASSERT(item_list_ != nullptr);
    if (item_list_ != nullptr)
    {
        if (allow_multi_select_)
        {
            AddCheckboxItem(item_text, user_data, true, false);
        }
        else
        {
            // Create labels for single-select comboboxes
            AddLabelItem(item_text, user_data);
        }
    }
}

QLabel* ArrowIconComboBox::AddLabelItem(const QString& item_text, const QVariant& user_data)
{
    // Labels should only be added to single-select comboboxes.
    Q_ASSERT(!allow_multi_select_);
    if (allow_multi_select_)
    {
        return nullptr;
    }

    QListWidgetItem* item = new QListWidgetItem();
    item->setFont(item_list_->font());

    if (user_data.isValid())
    {
        item->setData(Qt::UserRole, user_data);
    }

    // Create labels for single-select comboboxes
    ScaledLabel* label = new ScaledLabel(this);
    label->setFont(item_list_->font());
    label->setObjectName("label_option_" + QString::number(item_list_->count()));
    label->setText(item_text);
    item_list_->addItem(item);
    item_list_->setItemWidget(item, label);

    return label;
}

QCheckBox* ArrowIconComboBox::AddCheckboxItem(const QString& item_text, const QVariant& user_data, bool checked, bool is_all_option)
{
    // Checkboxes should only be added to multi-select comboboxes.
    Q_ASSERT(allow_multi_select_);
    if (!allow_multi_select_)
    {
        return nullptr;
    }

    QListWidgetItem* item = new QListWidgetItem();
    item->setFont(item_list_->font());

    if (user_data.isValid())
    {
        item->setData(Qt::UserRole, user_data);
    }

    ScaledCheckBox* checkbox = new ScaledCheckBox(item_text);
    checkbox->setEnabled(true);
    checkbox->setFont(item_list_->font());
    checkbox->setChecked(checked);
    checkbox->setCursor(Qt::PointingHandCursor);

    if (is_all_option)
    {
        // An "All" checkbox can only be added once. Assert this has not already been added.
        Q_ASSERT(all_choice_added_ == false);
        if (all_choice_added_ == false)
        {
            all_choice_added_ = true;

            item_list_->insertItem(kAllIndex, item);
            item_list_->setItemWidget(item, checkbox);
            checkbox->setObjectName("checkbox_option_all");
        }
    }
    else
    {
        item_list_->addItem(item);
        item_list_->setItemWidget(item, checkbox);
        checkbox->setObjectName("checkbox_option_" + QString::number(item_list_->count()));

        UpdateAllOption(checked);
    }

    // If this is an "All" checkbox, connect a slot to update the local variables and update the other checkboxes.
    if (is_all_option == true)
    {
        connect(checkbox, &QCheckBox::clicked, this, &ArrowIconComboBox::AllCheckboxClicked);
    }

    // Connect to clicked signal so that the "All" checkbox can be updated if needed.
    connect(checkbox, &QCheckBox::clicked, this, &ArrowIconComboBox::OnCheckboxClicked);

    return checkbox;
}

const QListWidgetItem* ArrowIconComboBox::FindItem(const QWidget* widget) const
{
    QListWidgetItem* current_item = nullptr;
    for (int i = 0; i < item_list_->count(); i++)
    {
        current_item = item_list_->item(i);
        if (item_list_->itemWidget(current_item) == widget)
        {
            break;
        }
    }
    return current_item;
}

QListWidgetItem* ArrowIconComboBox::FindItem(const int list_index) const
{
    Q_ASSERT(item_list_ != nullptr);
    Q_ASSERT(list_index >= 0 && list_index < item_list_->count());

    return item_list_->item(list_index);
}

bool ArrowIconComboBox::IsChecked(int list_index) const
{
    QListWidgetItem* item = item_list_->item(list_index);
    Q_ASSERT(item);

    QCheckBox* checkbox = qobject_cast<QCheckBox*>(item_list_->itemWidget(item));
    if (checkbox != nullptr)
    {
        return checkbox->isChecked();
    }
    return false;
}

bool ArrowIconComboBox::SetChecked(int list_index, bool checked)
{
    QListWidgetItem* item = item_list_->item(list_index);
    if (item != nullptr)
    {
        QCheckBox* checkbox = qobject_cast<QCheckBox*>(item_list_->itemWidget(item));
        if (checkbox != nullptr)
        {
            checkbox->setChecked(checked);
            return true;
        }
    }
    return false;
}

QVariant ArrowIconComboBox::ItemData(int index, int role) const
{
    Q_ASSERT(item_list_ != nullptr);

    if (item_list_ != nullptr)
    {
        if (index < item_list_->count() && index >= 0)
        {
            return item_list_->item(index)->data(role);
        }
    }

    return QVariant();
}

void ArrowIconComboBox::RemoveItem(const QString& item_string)
{
    Q_ASSERT(item_list_ != nullptr);

    if (item_list_ != nullptr)
    {
        const QList<QListWidgetItem*> list = item_list_->findItems(item_string, Qt::MatchExactly);
        foreach (QListWidgetItem* item, list)
        {
            // if removing the "All" item, remove the "All" functionality from the combo box
            if (all_choice_added_ == true && item_list_->item(kAllIndex) == item)
            {
                DisconnectSignals();
                all_choice_added_    = false;
                all_choice_selected_ = false;
            }
            item_list_->removeItemWidget(item);
            delete item;
        }
    }
}

void ArrowIconComboBox::SetAnchor(QPoint point)
{
    int height = item_list_->GetListWidgetHeight();
    int width  = item_list_->GetListWidgetWidth();
    item_list_->setGeometry(point.x(), point.y(), width, height);
}

void ArrowIconComboBox::DisconnectSignals()
{
    if (item_list_ != nullptr)
    {
        QListWidgetItem* current_item = nullptr;
        for (int i = 0; i < item_list_->count(); i++)
        {
            current_item        = item_list_->item(i);
            QCheckBox* checkbox = qobject_cast<QCheckBox*>(item_list_->itemWidget(current_item));
            if (checkbox != nullptr)
            {
                if (all_choice_added_ && i == kAllIndex)
                {
                    disconnect(checkbox, &QCheckBox::clicked, this, &ArrowIconComboBox::AllCheckboxClicked);
                }

                disconnect(checkbox, &QCheckBox::clicked, this, &ArrowIconComboBox::OnCheckboxClicked);
            }
        }
    }
}

void ArrowIconComboBox::OpenMenu()
{
    Q_ASSERT(item_list_ != nullptr);

    if (item_list_->count() > 0)
    {
        if (true == show_list_above_button_)
        {
            SetDirection(ArrowIconComboBox::Direction::DownArrow);
        }
        else
        {
            SetDirection(ArrowIconComboBox::Direction::UpArrow);
        }

        item_list_->UpdateSize();
        item_list_->show();

        emit Opened();
    }
}

void ArrowIconComboBox::CloseMenu()
{
    if (true == show_list_above_button_)
    {
        SetDirection(ArrowIconComboBox::Direction::UpArrow);
    }
    else
    {
        SetDirection(ArrowIconComboBox::Direction::DownArrow);
    }

    if (item_list_ != nullptr)
    {
        item_list_->hide();

        emit Closed();
    }
}

bool ArrowIconComboBox::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);

    if (event != nullptr)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            if (item_list_ != nullptr)
            {
                if ((this->underMouse() == false) && (item_list_->underMouse() == false))
                {
                    CloseMenu();
                }
            }
        }
    }

    return false;
}

QSize ArrowIconComboBox::sizeHint() const
{
    ensurePolished();

    ScalingManager& sm = ScalingManager::Get();

    // There are two offsets - one between the arrow and the text, and another after the text.
    const int total_scaled_text_offsets = 2.0 * sm.Scaled(kTextOffsetX);

    QSize size_hint;

    // Height is either: the size of the arrow, or the height of the text, whichever is greater.
    size_hint.setHeight(std::max<int>(size_, fontMetrics().height()));

    if (retain_default_text_)
    {
        // Width is arrow size + offset + current text size + offset.
        size_hint.setWidth(size_ + total_scaled_text_offsets + fontMetrics().horizontalAdvance(text()));
    }
    else if (item_list_ != nullptr)
    {
        // Find the longest item in the list.
        QListWidgetItem* current_item   = nullptr;
        int              max_item_width = 0;
        for (int i = 0; i < item_list_->count(); i++)
        {
            current_item   = item_list_->item(i);
            int item_width = fontMetrics().horizontalAdvance(ItemText(current_item));
            if (item_width > max_item_width)
            {
                max_item_width = item_width;
            }
        }

        int prefix_width = fontMetrics().horizontalAdvance(prefix_text_);

        // Width is arrow size + offset + optional prefix text + current text size + offset.
        size_hint.setWidth(size_ + total_scaled_text_offsets + prefix_width + max_item_width);
    }

    return size_hint;
}

void ArrowIconComboBox::ListItemClicked(QListWidgetItem* item)
{
    if (item != nullptr)
    {
        if (item->flags().testFlag(Qt::ItemFlag::ItemIsEnabled))
        {
            const QCheckBox* check_box = qobject_cast<QCheckBox*>(item_list_->itemWidget(item));

            // Don't set the selected text if this item is a checkbox.
            if (check_box == nullptr)
            {
                QString item_text = ItemText(item);
                SetSelectedText(item_text);

                if (allow_multi_select_ == false)
                {
                    CloseMenu();
                }

                // Emit selected item signal
                emit SelectedItem(item);
            }
        }
    }
}

void ArrowIconComboBox::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    if (item_list_ != nullptr)
    {
        if (item_list_->isVisible())
        {
            CloseMenu();
        }
        else
        {
            OpenMenu();
        }
    }
}

void ArrowIconComboBox::SetSelectedText(const QString& selection)
{
    // Only update displayed text if not retaining
    if (retain_default_text_ == false)
    {
        setText(prefix_text_ + selection);
    }

    selected_text_ = selection;

    emit SelectionChanged();
}

QString ArrowIconComboBox::ItemText(QListWidgetItem* list_item) const
{
    QString  item_text   = list_item->text();
    QWidget* item_widget = item_list_->itemWidget(list_item);
    if (item_text.isEmpty() && item_widget != nullptr)
    {
        ScaledLabel* item_label = dynamic_cast<ScaledLabel*>(item_widget);
        if (item_label != nullptr)
        {
            item_text = item_label->text();
        }
        else
        {
            ScaledCheckBox* item_checkbox = dynamic_cast<ScaledCheckBox*>(item_widget);
            if (item_checkbox != nullptr)
            {
                item_text = item_checkbox->text();
            }
            else
            {
                item_text = "unknown";
            }
        }
    }

    return item_text;
}

void ArrowIconComboBox::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    // Get the ScalingManager
    ScalingManager& sm = ScalingManager::Get();
    const int scaled_text_offset_x = sm.Scaled(kTextOffsetX);

    // Set up the painter
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.save();

    // Paint background
    painter.fillRect(rect(), palette().window());

    // Set properties for the lines
    QPen pen;

    if (isEnabled())
    {
        pen.setColor(color_);
    }
    else
    {
        pen.setColor(Qt::lightGray);
    }

    pen.setWidth(sm.Scaled(pen_width_));
    painter.setPen(pen);

    const int half_height     = height() / 2;
    const int half_arrow_size = size_ / 2;

    if (direction_ == Direction::DownArrow)
    {
        painter.translate(0, half_height - half_arrow_size);
    }
    else  // drawing an up arrow
    {
        // Rotate the paint object if drawing an up arrow
        painter.rotate(180);

        // Also translate the paint object towards the bottom of the rect
        painter.translate(-size_, -half_height - half_arrow_size);
    }

    // Draw the lines to generate the arrow
    painter.drawLine(vertices_[0], vertices_[1]);
    painter.drawLine(vertices_[1], vertices_[2]);

    // Now add the text
    // Restore the painter object before writing the text
    painter.restore();

    painter.setFont(this->font());
    if (isEnabled())
    {
        pen.setColor(font_color_);
    }
    else
    {
        pen.setColor(Qt::gray);
    }
    painter.setPen(pen);

    // Highlight substring if it is requested.
    if (highlight_substring_)
    {
        // Go through all highlight locations.
        for (const auto& string_highlight_data : string_highlight_data_)
        {
            QString current            = text().mid(0, string_highlight_data.start_location + 1);
            int     initial_text_width = painter.fontMetrics().horizontalAdvance(current);

            current = text().mid(string_highlight_data.start_location, string_highlight_data.end_location - string_highlight_data.start_location);
            if (!current.isNull())
            {
                int   width = painter.fontMetrics().boundingRect(current).width();
                QRect rect  = this->rect();
                rect.setX(rect.x() + size_ + scaled_text_offset_x + initial_text_width);
                rect.setWidth(width);
                painter.fillRect(rect, string_highlight_data.highlight_color);
            }
        }
    }

    // Draw the text.
    int half_font_height = painter.fontMetrics().capHeight() / 2;
    int text_base_y      = half_height + half_font_height;
    int text_base_x      = size_ + scaled_text_offset_x;
    painter.drawText(text_base_x, text_base_y, text());

    // Draw the borders of the widget on focus or mouse hover,
    // if it is requested.
    if (show_border_ && (hasFocus() || underMouse()))
    {
        pen.setColor(border_color_);
        pen.setWidth(sm.Scaled(1.0));
        painter.setPen(pen);
        painter.drawRect(this->rect());
    }

    painter.end();
}

void ArrowIconComboBox::SetBorderColor(const QColor& color)
{
    border_color_ = color;
}

void ArrowIconComboBox::SetShowBorder(bool value)
{
    show_border_ = value;
}

void ArrowIconComboBox::SetColor(const QColor& color)
{
    color_ = color;

    update();
}

void ArrowIconComboBox::SetFontColor(const QColor& color)
{
    font_color_ = color;

    update();
}

void ArrowIconComboBox::SetSize(int size)
{
    size_ = size;

    // Update the vertices
    CreateVertices();

    updateGeometry();
    update();
}

void ArrowIconComboBox::SetDirection(Direction direction)
{
    direction_ = direction;

    update();
}

void ArrowIconComboBox::SetPenWidth(const int width)
{
    pen_width_ = width;

    update();
}

void ArrowIconComboBox::SetHighLightSubString(bool value)
{
    highlight_substring_ = value;
}

void ArrowIconComboBox::SetHighLightSubStringData(QVector<StringHighlightData> string_highlight_data)
{
    string_highlight_data_ = string_highlight_data;
}

void ArrowIconComboBox::ClearHighLightSubStringData()
{
    string_highlight_data_.clear();
    string_highlight_data_.squeeze();
}

void ArrowIconComboBox::CreateVertices()
{
    // Based on the default coordinate system,
    // The origin is at the top left of the widget,
    // and positive numbers go to the lower right.
    // The vertices below define a down arrow.

    // Generate the vertices from the size input
    // Vertex zero is at the top right
    vertices_[0].setX(size_ * .8);
    vertices_[0].setY(size_ * .35);

    // Vertex 1 is halfway down the bottom side
    vertices_[1].setX(size_ / 2);
    vertices_[1].setY(size_ * .65);

    // Vertex 2 is the top left
    vertices_[2].setX(size_ * .2);
    vertices_[2].setY(size_ * .35);
}

void ArrowIconComboBox::focusInEvent(QFocusEvent* event)
{
    emit FocusInEvent();

    // Pass the event onto the base class.
    QPushButton::focusInEvent(event);
}

void ArrowIconComboBox::focusOutEvent(QFocusEvent* event)
{
    emit FocusOutEvent();

    // Pass the event onto the base class.
    QPushButton::focusOutEvent(event);
}
