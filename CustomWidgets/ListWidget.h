//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Definition of a custom ListWidget that handles loss of focus.
//=============================================================================

#ifndef _LIST_WIDGET_H_
#define _LIST_WIDGET_H_

#include <functional>

#include <QListWidget>

class ArrowIconWidget;

/// Class to implement custom list widget behavior. Monitor the FocusChanged signal
/// and hide itself when losing focus
class ListWidget : public QListWidget
{
    Q_OBJECT;

public:
    explicit ListWidget(QWidget* pParent, ArrowIconWidget* pButton, bool hide);
    virtual ~ListWidget();

    // Reimplement Qt's virtual methods
    void paintEvent(QPaintEvent* pEvent) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent* pEvent) Q_DECL_OVERRIDE;
    void focusInEvent(QFocusEvent* pEvent) Q_DECL_OVERRIDE;
    void focusOutEvent(QFocusEvent* pEvent) Q_DECL_OVERRIDE;

    // Static methods
    static void AddListWidgetCheckboxItem(const QString& text, ListWidget* &pListWidget, std::function<void(bool)> slotFunctionPointer, QWidget* pWidget, const QString& listName, const QString& boxName);
    static void UpdateAllCheckbox(ListWidget* &pListWidget);
    static void SetColumnVisibilityCheckboxes(ListWidget* &pListWidget, const std::vector<bool>& columnVisibility);

signals:
    void FocusInEvent();
    void FocusOutEvent();

private slots:
    void FocusChanged(QWidget* pOld, QWidget* pNow);

private:
    ArrowIconWidget*    m_pButton;      ///< The push button associated with this list box.
    bool                m_hide;         ///< The hide flag to hide the widget on focusChanged event.
    QWidget*            m_pParent;      ///< The parent object.
};

#endif // _LIST_WIDGET_H_
