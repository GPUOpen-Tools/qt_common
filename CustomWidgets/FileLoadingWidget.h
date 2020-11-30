//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for a file loading widget
//=============================================================================

#ifndef _FILE_LOADING_WIDGET_H_
#define _FILE_LOADING_WIDGET_H_

#include <QWidget>
#include <QMap>

class QTimer;
class AnimBars;

/// class to handle the file loading animation
class FileLoadingWidget : public QWidget
{
    Q_OBJECT

public:
    //Constructor
    FileLoadingWidget(QWidget* pParent = nullptr);
    ~FileLoadingWidget();

public slots:
    void Animate();

protected:
    //Qt's Paintevent
    void paintEvent(QPaintEvent* pEvent);

private:
    QTimer*   m_pUpdateTimer;             ///< timer to update widget
    AnimBars* m_pAnimBars;                ///< class containing the animating bars
};

#endif //_FILE_LOADING_WIDGET_H_
