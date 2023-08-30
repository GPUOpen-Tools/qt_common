//=============================================================================
/// Copyright (c) 2016-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a text search widget.
//=============================================================================

#include "text_search_widget.h"

#include "common_definitions.h"
#include "scaling_manager.h"

static const QString kTextSearchStylesheet =
    "QLineEdit {\
        border: 1px solid gray;}";

// Text search icon resource
static const QString kSearchIconResource = ":/Resources/assets/third_party/ionicons/search_icon.png";

TextSearchWidget::TextSearchWidget(QWidget* parent)
    : ScaledLineEdit(parent)
    , search_action_(nullptr)
{
    search_action_ = addAction(QIcon(kSearchIconResource), QLineEdit::TrailingPosition);

    setStyleSheet(kTextSearchStylesheet);

    // connect text changed signal/slot
    connect(this, &TextSearchWidget::textChanged, this, &TextSearchWidget::HandleTextChanged);

    // Set place holder text to search boxes
    setPlaceholderText("Search...");
}

TextSearchWidget::~TextSearchWidget()
{
}

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

void TextSearchWidget::ChangeIconToSearch()
{
    setClearButtonEnabled(false);
    delete search_action_;
    search_action_ = addAction(QIcon(kSearchIconResource), QLineEdit::TrailingPosition);
}

void TextSearchWidget::ChangeIconToDelete()
{
    setClearButtonEnabled(true);
    removeAction(search_action_);
}
