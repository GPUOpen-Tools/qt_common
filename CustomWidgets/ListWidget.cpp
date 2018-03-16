//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Implementation of a custom ListWidget that handles loss of focus
//=============================================================================

#include <cassert>

#include <QApplication>
#include <QPushButton>
#include <QKeyEvent>
#include <QCheckBox>

#include <ListWidget.h>
#include <ArrowIconWidget.h>
#include <../Scaling/ScalingManager.h>
#include <../Util/CommonDefinitions.h>
#include <../Util/QtUtil.h>

//-----------------------------------------------------------------------------
/// Explicit constructor.
/// \param pParent Pointer to the parent widget
//-----------------------------------------------------------------------------
ListWidget::ListWidget(QWidget* pParent, ArrowIconWidget* pButton, bool hide) :
    QListWidget(pParent),
    m_pButton(pButton),
    m_hide(hide),
    m_pParent(pParent)
{
    connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(FocusChanged(QWidget*, QWidget*)));
}

//-----------------------------------------------------------------------------
/// Destructor.
//-----------------------------------------------------------------------------
ListWidget::~ListWidget()
{
    disconnect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(FocusChanged(QWidget*, QWidget*)));
}

//-----------------------------------------------------------------------------
/// Slot to handle what happens when the list box loses focus
/// \param pOld Pointer to the widget losing focus
/// \param pNow Pointer to the widget getting focus
//-----------------------------------------------------------------------------
void ListWidget::FocusChanged(QWidget* pOld, QWidget* pNow)
{
    Q_UNUSED(pOld);
    Q_UNUSED(pNow);

    if (m_hide)
    {
        // Hide the list widget
        hide();

        // Set the button icon to down arrow
        m_pButton->SetDirection(ArrowIconWidget::Direction::DownArrow);
    }
}

//-----------------------------------------------------------------------------
/// Implementation of the paintEvent method for this class.
/// \param pEvent The Qt paint event
//-----------------------------------------------------------------------------
void ListWidget::paintEvent(QPaintEvent* pEvent)
{
    Q_UNUSED(pEvent);

    // Calculate the position
    QRect rect = m_pButton->geometry();
    QPoint pos(0, 0);
    pos = m_pButton->mapTo(m_pParent, pos);
    pos.setY(pos.y() + rect.height());

    // Calculate the width and the height of the list widget
    int height = QtCommon::QtUtil::GetListWidgetHeight(this);
    int width = QtCommon::QtUtil::GetListWidgetWidth(this);

    // Set the font size
    QFont font = this->font();
    font.setPixelSize(s_BUTTON_PIXEL_FONT_SIZE * ScalingManager::Get().GetScaleFactor());
    setStyleSheet(s_LIST_WIDGET_STYLE.arg(font.pixelSize()));

    // Set geometry
    setGeometry(pos.x(), pos.y(), width + s_CHECK_BOX_WIDTH, height);

    // Call the base class implementation
    QListWidget::paintEvent(pEvent);
}

//-----------------------------------------------------------------------------
/// Implementation of the keyPressEvent method for this class.
/// \param pEvent The Qt key event
//-----------------------------------------------------------------------------
void ListWidget::keyPressEvent(QKeyEvent* pEvent)
{
    // If the user pressed the escape key, close the list widget.
    if (pEvent->key() == Qt::Key_Escape)
    {
        emit m_pButton->clicked();
    }
    else
    {
        // For any other key, pass the event onto the base class implementation.
        QListWidget::keyPressEvent(pEvent);
    }
}

void ListWidget::UpdateAllCheckbox(ListWidget* &pListWidget)
{
    assert(pListWidget != nullptr);

    bool areAllItemsChecked = true;

    // Check to see if all of the boxes are checked.
    for (int i = 1; i < pListWidget->count(); i++)
    {
        QListWidgetItem* pItem = pListWidget->item(i);
        assert(pItem != nullptr);

        QCheckBox* pCheckBox = qobject_cast<QCheckBox*>(pListWidget->itemWidget(pItem));
        assert(pCheckBox != nullptr);

        if (pCheckBox->checkState() == Qt::CheckState::Unchecked)
        {
            areAllItemsChecked = false;
            break;
        }
    }

    // If update true, all boxes are checked, so update the All check box.
    QListWidgetItem* pItem = pListWidget->item(0);
    if (pItem != nullptr)
    {
        QCheckBox* pCheckBox = qobject_cast<QCheckBox*>(pListWidget->itemWidget(pItem));
        if (pCheckBox != nullptr)
        {
            Qt::CheckState state = areAllItemsChecked ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
            pCheckBox->setCheckState(state);
        }
    }
}

void ListWidget::SetColumnVisibilityCheckboxes(ListWidget* &pListWidget, const std::vector<bool>& columnVisibility)
{
    assert(pListWidget != nullptr);

    // The count of incoming visibility flags must match the number of list items + 1, to account for "All".
    int columnCount = pListWidget->count();
    assert(columnVisibility.size() == (size_t)(columnCount - 1));

    // Start at the 1st item instead of the 0th in order to skip the "All" check box item.
    for (int columnIndex = 1; columnIndex < columnCount; columnIndex++)
    {
        QListWidgetItem* pItem = pListWidget->item(columnIndex);
        if (pItem != nullptr)
        {
            QCheckBox* pCheckBox = qobject_cast<QCheckBox*>(pListWidget->itemWidget(pItem));
            if (pCheckBox != nullptr)
            {
                // Offset the column index to account for the extra "All" item.
                bool isVisible = columnVisibility[columnIndex - 1];
                pCheckBox->setCheckState(isVisible ? Qt::Checked : Qt::Unchecked);
            }
        }
    }

    ListWidget::UpdateAllCheckbox(pListWidget);
}

void ListWidget::AddListWidgetCheckboxItem(const QString& text, ListWidget* &pListWidget, std::function<void(bool)> slotFunctionPointer, QWidget* pWidget, const QString& listName, const QString& boxName)
{
    assert(pListWidget != nullptr);

    // Add each item as a check box.
    QListWidgetItem* pItem = new QListWidgetItem;
    pListWidget->addItem(pItem);
    QCheckBox* pCheckBox = new QCheckBox(text);
    pCheckBox->setCheckState(Qt::Checked);
    pCheckBox->setObjectName(boxName);
    pListWidget->setItemWidget(pItem, pCheckBox);

    // Set the mouse cursor to pointing hand cursor.
    pCheckBox->setCursor(Qt::PointingHandCursor);

    // Connect to this check box's stateChanged signal for the appropriate list widget.
    if (pListWidget->objectName().compare(listName) == 0)
    {
        bool isConnected = connect(pCheckBox, &QCheckBox::clicked, pWidget, slotFunctionPointer);
        assert(isConnected);
    }
    else
    {
        // Tried to add a check box to the wrong widget.
        assert(false);
    }
}

void ListWidget::focusInEvent(QFocusEvent* pEvent)
{
    emit FocusInEvent();

    // Pass the event onto the base class.
    QListWidget::focusInEvent(pEvent);
}

void ListWidget::focusOutEvent(QFocusEvent* pEvent)
{
    emit FocusOutEvent();

    // Pass the event onto the base class.
    QListWidget::focusOutEvent(pEvent);
}