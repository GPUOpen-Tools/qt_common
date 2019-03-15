//=============================================================================
/// Copyright (c) 2016-2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for the text search widget.
//=============================================================================

#ifndef _TEXT_SEARCH_WIDGET_H_
#define _TEXT_SEARCH_WIDGET_H_

#include <QLineEdit>
#include <QObject>
#include <QAction>

class TextSearchWidget : public QLineEdit
{
    Q_OBJECT

public:
    //Constructor
    TextSearchWidget(QWidget* pParent = nullptr);
	virtual ~TextSearchWidget() {}

private slots:
    void HandleTextChanged(const QString& text);

private:
    void ChangeIconToSearch();
    void ChangeIconToDelete();

    QAction* m_pSearchText;                        ///< action item to search text
};

#endif // _TEXT_SEARCH_WIDGET_H_
