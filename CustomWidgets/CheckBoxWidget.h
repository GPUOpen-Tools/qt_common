//=============================================================================
/// Copyright (c) 2016-2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for a check box widget.
//=============================================================================
#ifndef _CHECKBOX_WIDGET_H_
#define _CHECKBOX_WIDGET_H_

#include <QObject>
#include <QCheckBox>

class CheckBoxWidget: public QCheckBox
{
    Q_OBJECT

public:
    explicit CheckBoxWidget(QWidget* pParent = 0);
	virtual ~CheckBoxWidget() {}

    //Setters
    void setOnText(const QString& checkboxText);
    void setOffText(const QString& checkboxText);

private :
    QString m_onText;                   ///< text of checkbox in checked state
    QString m_offText;                  ///< text of checkbox in checked state

private slots:
    void stateChanged(int checkboxState);
};

#endif // _CHECKBOX_WIDGET_H_
