//=============================================================================
/// Copyright (c) 2016-2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of a check box widget.
//=============================================================================

#include "CheckBoxWidget.h"
#include <QFile>
#include <../Scaling/ScalingManager.h>
#include <../Util/CommonDefinitions.h>

//-----------------------------------------------------------------------------
/// Explicit constructor
/// \param parent The checkbox's parent
//-----------------------------------------------------------------------------
CheckBoxWidget::CheckBoxWidget(QWidget* pParent):
    QCheckBox(pParent)
{
    m_onText = gs_QT_STRING_ON;
    m_offText = gs_QT_STRING_OFF;
    setText(m_offText);

    connect(this, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
}

//-----------------------------------------------------------------------------
/// Implementation of setter method for checked state text.
/// \param checkboxText - Text of checkbox in checked state
//-----------------------------------------------------------------------------
void CheckBoxWidget::setOnText(const QString &checkboxText)
{
    m_onText = checkboxText;
}

//-----------------------------------------------------------------------------
/// Implementation of setter method for un-checked state text.
/// \param checkboxText - Text of checkbox in un-checked state
//-----------------------------------------------------------------------------
void CheckBoxWidget::setOffText(const QString &checkboxText)
{
    m_offText = checkboxText;
}

//-----------------------------------------------------------------------------
/// Implementation of state change for this item.
/// \param checkboxState - The current state of checkbox
//-----------------------------------------------------------------------------
void CheckBoxWidget::stateChanged(int checkboxState)
{
    Q_UNUSED(checkboxState);

    if(checkState() == Qt::Checked)
    {
        setText(m_onText);
    }
    else if(checkState() == Qt::Unchecked)
    {
        setText(m_offText);
    }
}
