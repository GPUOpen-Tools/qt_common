//=============================================================================
/// Copyright (c) 2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QMenuBar objects.
//=============================================================================
#include "scaled_menu_bar.h"

#include "scaling_manager.h"

ScaledMenuBar::ScaledMenuBar(QWidget* parent)
    : QMenuBar(parent)
{
    connect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledMenuBar::OnScaleFactorChanged);
}

ScaledMenuBar::~ScaledMenuBar()
{
    disconnect(&ScalingManager::Get(), &ScalingManager::ScaleFactorChanged, this, &ScaledMenuBar::OnScaleFactorChanged);
}

void ScaledMenuBar::OnScaleFactorChanged()
{
    // Need to change the fontsize on the MenuBar, then change it back in order to invalidate
    // the fontMetrics and get the size to recalculate
    QFont tmp_font  = font();
    qreal pointsize = tmp_font.pointSizeF();

    // Make sure it changes to something different
    tmp_font.setPointSize(pointsize + 1);
    setFont(tmp_font);
    UpdateActionsFont(actions(), tmp_font);

    // Now change it back.
    // This will automatically cause the menu to recalculate its size.
    tmp_font.setPointSize(pointsize);
    setFont(tmp_font);
    UpdateActionsFont(actions(), tmp_font);
}

void ScaledMenuBar::UpdateActionsFont(QList<QAction*> action_list, const QFont& font)
{
    foreach (QAction* action, action_list)
    {
        action->setFont(font);
        QMenu* menu = action->menu();
        if (menu != nullptr)
        {
            menu->setFont(font);

            // Update submenu actions
            UpdateActionsFont(menu->actions(), font);
        }
    }
}
