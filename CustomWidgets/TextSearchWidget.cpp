//=============================================================================
/// Copyright (c) 2016-2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a text search widget.
//=============================================================================

#include "TextSearchWidget.h"

#include <../Util/CommonDefinitions.h>

static const QString s_TEXT_SEARCH_STYLESHEET = "QLineEdit {\
        border: 1px solid gray;}";

//Text search icon resource
static const QString gs_QT_STRING_SEARCH_RESOURCE = ":/Resources/assets/third_party/ionicons/search_icon.png";

//-----------------------------------------------------------------------------
/// constructor
/// \param pParent - The parent of this widget
//-----------------------------------------------------------------------------
TextSearchWidget::TextSearchWidget(QWidget* pParent):
    QLineEdit(pParent),
    m_pSearchText(nullptr)
{
    m_pSearchText = addAction(QIcon(gs_QT_STRING_SEARCH_RESOURCE), QLineEdit::TrailingPosition);

    setStyleSheet(s_TEXT_SEARCH_STYLESHEET);

    // connect text changed signal/slot
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(HandleTextChanged(const QString&)));

    // Set place holder text to search boxes
    setPlaceholderText("Search...");
}

//-----------------------------------------------------------------------------
/// Change icon
/// \param text string containing the text entered
//-----------------------------------------------------------------------------
void TextSearchWidget::HandleTextChanged(const QString& text)
{
    // change the icon depending on the contents of the search box
    if (text.isEmpty())
    {
        ChangeIconToSearch();
    }
    else
    {
        ChangeIconToDelete();
    }

}
//-----------------------------------------------------------------------------
/// Change icon to magnifier icon
//-----------------------------------------------------------------------------
void TextSearchWidget::ChangeIconToSearch()
{
    setClearButtonEnabled(false);
    delete m_pSearchText;
    m_pSearchText = addAction(QIcon(gs_QT_STRING_SEARCH_RESOURCE), QLineEdit::TrailingPosition);
}

//-----------------------------------------------------------------------------
/// Change icon to "x" icon
//-----------------------------------------------------------------------------
void TextSearchWidget::ChangeIconToDelete()
{
    setClearButtonEnabled(true);
    removeAction(m_pSearchText);
}
