//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Implementation for a mini recent trace widget.
//=============================================================================

#include "RecentTraceMiniWidget.h"

#include <../Scaling/ScalingManager.h>
#include <../Util/CommonDefinitions.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QSignalMapper>
#include <QDir>

#include <time.h>

//-----------------------------------------------------------------------------
/// Explicit constructor.
/// \param pParent The color picker widget's parent.
//-----------------------------------------------------------------------------
RecentTraceMiniWidget::RecentTraceMiniWidget(QWidget* pParent) :
    QWidget(pParent),
    m_pSignalMapper(new QSignalMapper(this)),
    m_pPathButton(new QPushButton(this))
{
    m_pPathButton->setCursor(Qt::PointingHandCursor);
    m_pPathButton->setStyleSheet(gs_LINK_BUTTON_STYLESHEET);

    m_pVerticalLayout = new QVBoxLayout(this);
    m_pVerticalLayout->setContentsMargins(0, 0, 0, 0);

    QFont font;
    font.setPointSize(10);

    m_pPathButton->setFont(font);

    // Additional settings for the path button
    QSizePolicy policy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pPathButton->setSizePolicy(policy);

    m_pVerticalLayout->addWidget(m_pPathButton);
}

//-----------------------------------------------------------------------------
/// Destructor.
//-----------------------------------------------------------------------------
RecentTraceMiniWidget::~RecentTraceMiniWidget()
{
    delete m_pSignalMapper;
    delete m_pPathButton;
}

//-----------------------------------------------------------------------------
/// Set the trace information for this widget from a RecentFileData object.
/// \param file The file this widget is going to show trace data from.
//-----------------------------------------------------------------------------
void RecentTraceMiniWidget::SetFile(const RecentFileData& file)
{
    // Get file parameters. Convert file from its native separators
    QString path = QDir::fromNativeSeparators(file.path);
    QString fileName = path.mid(path.lastIndexOf("/") + 1, path.length());

    // Set label text as just the filename and tooltip as full path
    m_pPathButton->setText(fileName);
    m_pPathButton->setToolTip(path);

    // Connect path label clicked() signal to this widgets clicked(QString) signal
    m_pSignalMapper->setMapping(m_pPathButton, path);
    connect(m_pPathButton, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    connect(m_pSignalMapper, SIGNAL(mapped(QString)), this, SIGNAL(clicked(QString)));
}
