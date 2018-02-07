//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Definition of a custom ListWidget that handles loss of focus.
//=============================================================================

#ifndef _LIST_WIDGET_H_
#define _LIST_WIDGET_H_

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
    virtual void paintEvent(QPaintEvent* pEvent);

private slots:
    void FocusChanged(QWidget* pOld, QWidget* pNow);

private:
    ArrowIconWidget*    m_pButton;      ///< The push button associated with this list box.
    bool                m_hide;         ///< The hide flag to hide the widget on focusChanged event.
    QWidget*            m_pParent;      ///< The parent object.
};

#endif // _LIST_WIDGET_H_