//=============================================================================
/// Copyright (c) 2020-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling wrapper for QMenuBar object.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SCALED_MENUBAR_H_
#define QTCOMMON_CUSTOM_WIDGETS_SCALED_MENUBAR_H_

#include <QMenuBar>

/// Custom QMenuBar that supports DPI Scale changes
class ScaledMenuBar : public QMenuBar
{
    Q_OBJECT

public:
    /// Overridden Constructor
    /// \param parent The parent of this menubar
    ScaledMenuBar(QWidget* parent = nullptr);

    /// Virtual Destructor
    virtual ~ScaledMenuBar();

private slots:
    /// Callback when the scale factor changes
    void OnScaleFactorChanged();

private:
    /// Recursively update the font on the actions and associated menus.
    /// \param action_list The list of actions that need updated fonts
    /// \param font The font to assign to each action and menu
    void UpdateActionsFont(QList<QAction*> action_list, const QFont& font);
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SCALED_MENUBAR_H_
