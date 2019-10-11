//=============================================================================
/// Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of our down arrow combobox
//=============================================================================

#include "ArrowIconComboBox.h"

#include <QApplication>
#include <ListWidget.h>

//-----------------------------------------------------------------------------
/// Constructor for ArrowIconComboBox.
/// \param pParent The parent widget
//-----------------------------------------------------------------------------
ArrowIconComboBox::ArrowIconComboBox(QObject* pParent)
{
    Q_UNUSED(pParent);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ArrowIconComboBox::~ArrowIconComboBox()
{
    delete m_pItemList;
}

//-----------------------------------------------------------------------------
/// Init the widget
/// \param pParent The parent widget
/// \param defaultText The starting string
/// \param whether want this guy to remain open as multiple things are selected
//-----------------------------------------------------------------------------
void ArrowIconComboBox::Init(QWidget* pParent, const QString& defaultText, bool multiSelect)
{
    // create list widget for the combo box
    m_pItemList = new ListWidget(pParent, this, false);

    // also disable scrollbars on this list widget
    m_pItemList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_pItemList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // start out the combo box with the first entry
    m_pItemList->setCurrentRow(0);
    m_pItemList->hide();

    m_multiSelect = multiSelect;
    m_defaultText = defaultText;

    ResetSelection();

    if (multiSelect == true)
    {
        m_pItemList->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);
    }

    connect(m_pItemList, &ListWidget::itemClicked, this, &ArrowIconComboBox::ListItemClicked);

    qApp->installEventFilter(this);
}

//-----------------------------------------------------------------------------
/// Update widget state
//-----------------------------------------------------------------------------
void ArrowIconComboBox::Update()
{
    if (m_pItemList->count() == 0)
    {
        CloseMenu();

        ResetSelection();
    }
}

//-----------------------------------------------------------------------------
/// Reset to starting state
//-----------------------------------------------------------------------------
void ArrowIconComboBox::ResetSelection()
{
    SetSelectedText(m_defaultText);
}

//-----------------------------------------------------------------------------
/// Get previous row index in list
/// \pre The combobox must be initialized
/// \return item list previous row index or current if at beginning
//-----------------------------------------------------------------------------
int ArrowIconComboBox::PrevRow() const
{
    /// the item list must not be null
    Q_ASSERT(m_pItemList != nullptr);

    const int currentRow = m_pItemList->currentRow();
    const int prevRow = currentRow - 1;
    if (prevRow < 0)
    {
        return m_pItemList->currentRow();
    }
    else
    {
        return prevRow;
    }
}

//-----------------------------------------------------------------------------
/// Get current row index in list
/// \pre The combobox must be initialized
/// \return item list current row index
//-----------------------------------------------------------------------------
int ArrowIconComboBox::CurrentRow() const
{
    /// the item list must not be null
    Q_ASSERT(m_pItemList != nullptr);

    return m_pItemList->currentRow();
}

//-----------------------------------------------------------------------------
/// Get next row index in list
/// \pre The combobox must be initialized
/// \return item list next row index or current if at end of list
//-----------------------------------------------------------------------------
int ArrowIconComboBox::NextRow() const
{
    /// the item list must not be null
    Q_ASSERT(m_pItemList != nullptr);

    const int currentRow = m_pItemList->currentRow();
    const int nextRow = currentRow + 1;
    if (nextRow >= m_pItemList->count())
    {
        return m_pItemList->currentRow();
    }
    else
    {
        return nextRow;
    }
}

//-----------------------------------------------------------------------------
/// Get the number of rows
/// \return item list row count
//-----------------------------------------------------------------------------
int ArrowIconComboBox::RowCount() const
{
    if (m_pItemList != nullptr)
    {
        return m_pItemList->count();
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------
/// Set current row
/// \param idx the list index to select
//-----------------------------------------------------------------------------
void ArrowIconComboBox::SetSelectedRow(int idx)
{
    if (idx >= 0 && idx < m_pItemList->count())
    {
        QListWidgetItem* pListItem = m_pItemList->item(idx);

        if (pListItem != nullptr)
        {
            setText(pListItem->text());
            m_selectedText = pListItem->text();

            m_pItemList->setCurrentRow(idx);
        }
    }
}

//-----------------------------------------------------------------------------
/// Specify a maximum height
/// \param height new height
//-----------------------------------------------------------------------------
void ArrowIconComboBox::SetMaximumHeight(int height)
{
    m_pItemList->setMaximumHeight(height);
}

//-----------------------------------------------------------------------------
/// Specify a fixed width
/// \param width new width
//-----------------------------------------------------------------------------
void ArrowIconComboBox::SetFixedWidth(int width)
{
    setFixedWidth(width);

    m_pItemList->setFixedWidth(width);
}

//-----------------------------------------------------------------------------
/// Throw it all out
//-----------------------------------------------------------------------------
void ArrowIconComboBox::ClearItems()
{
    m_pItemList->clear();
}

//-----------------------------------------------------------------------------
/// Add a new list item.
/// \param pItem the item to be added to the list.
//-----------------------------------------------------------------------------
void ArrowIconComboBox::AddItem(QListWidgetItem* pItem)
{
    Q_ASSERT(pItem != nullptr);

    m_pItemList->addItem(pItem);
}

//-----------------------------------------------------------------------------
/// Add a new list item.
/// \param newItem The string to be added to the list.
//-----------------------------------------------------------------------------
void ArrowIconComboBox::AddItem(const QString& newItem)
{
    m_pItemList->addItem(newItem);
}

//-----------------------------------------------------------------------------
/// Add a new list item.
/// \param newItem The string to be added to the list.
/// \param userData User data for item.
/// \param role Data role defaults to Qt::UserRole.
//-----------------------------------------------------------------------------
void ArrowIconComboBox::AddItem(const QString& newItem, const QVariant& userData, int role)
{
    QListWidgetItem* pItem = new QListWidgetItem;
    pItem->setText(newItem);
    pItem->setData(role, userData);

    m_pItemList->addItem(pItem);
}

//-----------------------------------------------------------------------------
/// Gets data stored in item at specified index
/// \param index the item index
/// \param role Data role defaults to Qt::UserRole.
//-----------------------------------------------------------------------------
QVariant ArrowIconComboBox::ItemData(int index, int role)
{
    if (index < m_pItemList->count() && index >= 0)
    {
        return m_pItemList->item(index)->data(role);
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
/// Remove a list item.
/// \param itemString The string to be removed from the list.
//-----------------------------------------------------------------------------
void ArrowIconComboBox::RemoveItem(const QString& itemString)
{
    const QList<QListWidgetItem*> itemList = m_pItemList->findItems(itemString, Qt::MatchExactly);
    foreach(QListWidgetItem* pItem, itemList)
    {
        m_pItemList->removeItemWidget(pItem);
        delete pItem;
    }
}

//-----------------------------------------------------------------------------
/// Open the list
//-----------------------------------------------------------------------------
void ArrowIconComboBox::OpenMenu()
{
    if (m_pItemList->count() > 0)
    {
        SetDirection(ArrowIconWidget::UpArrow);

        m_pItemList->show();
    }
}

//-----------------------------------------------------------------------------
/// Close the list
//-----------------------------------------------------------------------------
void ArrowIconComboBox::CloseMenu()
{
    SetDirection(ArrowIconWidget::DownArrow);
    m_pItemList->hide();
}

//-----------------------------------------------------------------------------
/// Event filter to make the list close when clicking outside of widget
/// \param pObject the test object
/// \param pEvent the actual event
/// \return pass or fail
//-----------------------------------------------------------------------------
bool ArrowIconComboBox::eventFilter(QObject* pObject, QEvent* pEvent)
{
    Q_UNUSED(pObject);

    if (pEvent->type() == QEvent::MouseButtonPress)
    {
        if ((this->underMouse() == false) && (m_pItemList->underMouse() == false))
        {
            CloseMenu();
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
/// Provide a sizeHint which includes the arrow and the text width.
//-----------------------------------------------------------------------------
QSize ArrowIconComboBox::sizeHint() const
{
    return ArrowIconWidget::sizeHint();
}

//-----------------------------------------------------------------------------
/// Handle what happens when a list item is clicked
/// \param pItem the clicked item
//-----------------------------------------------------------------------------
void ArrowIconComboBox::ListItemClicked(QListWidgetItem* pItem)
{
    if (pItem != nullptr)
    {
        SetSelectedText(pItem->text());

        if (m_multiSelect == false)
        {
            CloseMenu();
        }

        // Emit selected item signal
        emit SelectedItem(pItem);
    }
}

//-----------------------------------------------------------------------------
/// Toggle the list when the master button is clicked
/// \param pItem the clicked item
//-----------------------------------------------------------------------------
void ArrowIconComboBox::mousePressEvent(QMouseEvent* pEvent)
{
    Q_UNUSED(pEvent);

    if (m_pItemList->isVisible())
    {
        CloseMenu();
    }
    else
    {
        OpenMenu();
    }
}

//-----------------------------------------------------------------------------
/// Cache the last user selection
/// \param selection the selected text
//-----------------------------------------------------------------------------
void ArrowIconComboBox::SetSelectedText(const QString& selection)
{
    setText(selection);
    m_selectedText = selection;

    emit SelectionChanged();
}
