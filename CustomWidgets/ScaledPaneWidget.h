//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for ScaledPaneWidget
//=============================================================================

#ifndef _SCALED_PANE_WIDGET_H_
#define _SCALED_PANE_WIDGET_H_

#include <QWidget>

/// Support for ScaledPaneWidget
class ScaledPaneWidget : public QWidget
{
    Q_OBJECT;

public:
    explicit ScaledPaneWidget(QWidget* pParent = nullptr);
    ~ScaledPaneWidget();

    virtual void setGeometry(const QRect& rect);
    virtual void setMinimumSize(const QSize& size);
    virtual void setMaximumSize(const QSize& size);
    virtual void setStyleSheet(const QString& styleSheet);

public slots:
    void adjustScaling(double oldScaleFactor, double newScaleFactor);

private:

    QRect m_baseGeometry;           ///< Base set geometry (before applying scaling)
    QSize m_baseMinSize;            ///< Base set min size (before applying scaling)
    QSize m_baseMaxSize;            ///< Base set max size (before applying scaling)
    QString m_baseStyleSheet;       ///< Base set stylesheet (before applying scaling)

};

#endif // _SCALED_PANE_WIDGET_H_
