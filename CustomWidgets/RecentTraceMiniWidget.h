//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for a mini recent trace widget.
//=============================================================================

#ifndef _RECENT_TRACE_MINI_WIDGET_H_
#define _RECENT_TRACE_MINI_WIDGET_H_

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSignalMapper>
#include <QVBoxLayout>

#include <ScaledPaneWidget.h>
#include <../Util/CommonDefinitions.h>

/// Widget that represents a single trace in the recent traces pane
class RecentTraceMiniWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RecentTraceMiniWidget(QWidget* pParent = nullptr);
    virtual ~RecentTraceMiniWidget();

    void SetFile(const RecentFileData& file);
    int GetElementHeight() { return m_pPathButton->height() + m_pVerticalLayout->spacing(); }

private:
    QSignalMapper*    m_pSignalMapper;     ///< Path label signal mapper
    QPushButton*      m_pPathButton;       ///< Button with file path text
    QVBoxLayout*      m_pVerticalLayout;   ///< This widget's layout containing all stuff

signals:
    void clicked(QString);
};

#endif // _RECENT_TRACE_MINI_WIDGET_H_
