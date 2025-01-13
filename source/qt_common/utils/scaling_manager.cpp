//=============================================================================
/// Copyright (c) 2016-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation of the ScalingManager. The ScalingManager handles
/// scaling operations such as providing the current scale factor and emitting
/// the ScaleFactorChanged signal for other QObjects to utilize.
//=============================================================================

#include "scaling_manager.h"

#include <QApplication>
#include <QDebug>
#include <QLayout>
#include <QScreen>
#include <QWidget>
#include <QWindow>

#ifndef __APPLE__
const static int kStandardDpi = 96;
#endif

ScalingManager::ScalingManager()
    : main_widget_(nullptr)
{
#ifdef __APPLE__
    // MacOSX handles scaling - so force to 1.0
    scale_factor_ = 1.0;
#else
    dpi_          = QApplication::primaryScreen()->logicalDotsPerInch();
    scale_factor_ = (double)dpi_ / (double)kStandardDpi;
    initial_dpi_  = dpi_;
#endif
    rescale_factor_ = 1.0;

    // Connect signals
    connect(qApp, &QApplication::screenAdded, this, &ScalingManager::OnScreenAdded);
    connect(qApp, &QApplication::screenRemoved, this, &ScalingManager::OnScreenRemoved);
}

ScalingManager::~ScalingManager()
{
}

ScalingManager& ScalingManager::Get()
{
    static ScalingManager s_Instance;
    return s_Instance;
}

#ifdef _DEBUG
qreal ScalingManager::GetPhysicalDpi()
{
    qreal dpi;

    if (main_widget_ == nullptr)
    {
        dpi = QApplication::primaryScreen()->physicalDotsPerInch();
    }
    else
    {
        dpi = main_widget_->windowHandle()->screen()->physicalDotsPerInch();
    }

    return dpi;
}
#endif  // _DEBUG

void ScalingManager::Initialize(QWidget* main_widget)
{
    QWindow* window_handle = nullptr;

    main_widget_ = main_widget;
    if (main_widget != nullptr)
    {
        window_handle = main_widget->windowHandle();
    }

    // Screen DPI changed signals/slots
    for (QScreen* screen : QApplication::screens())
    {
        connect(screen, &QScreen::logicalDotsPerInchChanged, this, &ScalingManager::OnDpiChanged);
    }

    // Screen changed and added signals/slots
    connect(window_handle, &QWindow::screenChanged, this, &ScalingManager::OnScreenChanged);

    // DPI update for current screen
    if (window_handle != nullptr && window_handle->screen() != nullptr)
    {
        initial_dpi_ = window_handle->screen()->logicalDotsPerInch();

        UpdateScaleFactor(initial_dpi_);
    }
}

void ScalingManager::OnDpiChanged(qreal dpi)
{
    QObject* sender         = this->sender();
    QScreen* sender_monitor = qobject_cast<QScreen*>(sender);
    QWindow* window_handle  = main_widget_->windowHandle();

    if (window_handle != nullptr)
    {
        QScreen* main_window_monitor = window_handle->screen();

        if (!sender_monitor || (main_window_monitor == sender_monitor))
        {
            // If dpi changes, trigger a scale factor change
            if (dpi_ != dpi)
            {
                UpdateScaleFactor(dpi);
            }
        }
    }
}

void ScalingManager::UpdateScaleFactor(qreal dpi)
{
    dpi_                    = dpi;
    double old_scale_factor = scale_factor_;

#ifdef __APPLE__
    // MacOSX handles scaling - so force to 1.0
    scale_factor_ = 1.0;
#else
    scale_factor_ = (double)dpi_ / (double)kStandardDpi;
#endif
    rescale_factor_ = scale_factor_ / old_scale_factor;

    emit ScalingManager::ScaleFactorChanged();
}

void ScalingManager::OnScreenChanged(QScreen* screen)
{
    if (screen != nullptr)
    {
        // DPI update
        UpdateScaleFactor(screen->logicalDotsPerInch());
    }
}

void ScalingManager::OnScreenAdded(QScreen* screen)
{
    if (screen != nullptr)
    {
        // Connect up signals/slots for new screen
        connect(screen, &QScreen::logicalDotsPerInchChanged, this, &ScalingManager::OnDpiChanged);
    }
}

void ScalingManager::OnScreenRemoved(QScreen* screen)
{
    if (screen != nullptr)
    {
        // Disconnect up signals/slots for removed screen
        disconnect(screen, &QScreen::logicalDotsPerInchChanged, this, &ScalingManager::OnDpiChanged);
    }
}
