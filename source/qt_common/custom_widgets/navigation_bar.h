//=============================================================================
/// Copyright (c) 2017-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for the custom browser navigation tool bar.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_NAVIGATION_BAR_H_
#define QTCOMMON_CUSTOM_WIDGETS_NAVIGATION_BAR_H_

#include <QHBoxLayout>
#include <QWidget>

#include "icon_button.h"

/// @brief NavigationBar is a composite QWidget made of 2 icon buttons, meant to be used for navigation.
class NavigationBar : public QWidget
{
    Q_OBJECT

public:
    /// Explicit constructor
    /// \param parent The NavigationBar's parent widget.
    explicit NavigationBar(QWidget* parent = nullptr);

    /// Virtual destructor
    virtual ~NavigationBar()
    {
    }

    /// Accessor for the back button.
    /// \return The IconButton which represents the back button.
    IconButton& BackButton()
    {
        return browse_back_button_;
    }

    /// Accessor for the fowrad button.
    /// \return The IconButton which represents the forward button.
    IconButton& ForwardButton()
    {
        return browse_forward_button_;
    }

public slots:
    /// Enable or disable the back button.  Set the mouse cursor to pointing hand
    /// or an arrow.
    /// \param enable Set to true to enable the button or false to disable it.
    void EnableBackButton(bool enable);

    /// Enable or disable the forward button.  Set the mouse cursor to pointing hand
    /// or an arrow.
    /// \param enable Set to true to enable the button or false to disable it.
    void EnableForwardButton(bool enable);

protected:
    /// Handle mouse move event.  Change cursor back to an arrow.
    /// \param event The mouseMoveEvent event.
    virtual void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

    QHBoxLayout layout_;                 ///< The layout for the navigation bar.
    IconButton  browse_back_button_;     ///< The browse back navigation button.
    IconButton  browse_forward_button_;  ///< The browse forward navigation button.

private:
    const QString kNavigationButtonStyle_ =
        "IconButton {  margin: 0px; padding: 0px; border: none; background-color: rgb(51,51,51); background: rgb(51,51,51); color: #666; } ";
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_NAVIGATION_BAR_H_
