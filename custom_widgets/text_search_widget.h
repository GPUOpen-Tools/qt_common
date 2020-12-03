//=============================================================================
/// Copyright (c) 2016-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for the text search widget.
//=============================================================================

#ifndef QTCOMMON_CUSTOMWIDGET_TEXT_SEARCH_WIDGET_H_
#define QTCOMMON_CUSTOMWIDGET_TEXT_SEARCH_WIDGET_H_

#include <QObject>
#include <QAction>

#include "scaled_line_edit.h"

class TextSearchWidget : public ScaledLineEdit
{
    Q_OBJECT

public:
    /// Constructor
    /// \param parent The parent of this widget.
    TextSearchWidget(QWidget* parent = nullptr);

    /// Destructor
    virtual ~TextSearchWidget();

private slots:
    /// Change icon when the text is changed.
    /// \param text String containing the text entered.
    void HandleTextChanged(const QString& text);

private:
    /// Change icon to magnifier icon.
    void ChangeIconToSearch();

    /// Change icon to "x" icon.
    void ChangeIconToDelete();

    QAction* search_action_;  ///< Action item to search text
};

#endif  // QTCOMMON_CUSTOMWIDGET_TEXT_SEARCH_WIDGET_H_
