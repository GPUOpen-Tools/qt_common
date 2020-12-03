//=============================================================================
/// Copyright (c) 2017-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for the custom browser navigation tool bar.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_NAVIGATION_BAR_H_
#define QTCOMMON_CUSTOM_WIDGETS_NAVIGATION_BAR_H_

#include <QAbstractButton>
#include <QHBoxLayout>

#include "icon_button.h"

/// NavigationBar widget. Customized toolbar for browser navigation.
class NavigationBar : public QAbstractButton
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

    /// Overridden paint event, which just does default painting.
    /// \param event The paintEvent
    virtual void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE
    {
        QWidget::paintEvent(event);
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

private:
    const QString kNavigationButtonStyle_ =
        "IconButton {  margin: 0px; padding: 0px; border: none; background-color: rgb(51,51,51); background: rgb(51,51,51); color: #666; } ";

    const QString kBrowseBackNormalResource_      = ":/Resources/assets/browse_back_normal.svg";
    const QString kBrowseBackPressedResource_     = ":/Resources/assets/browse_back_pressed.svg";
    const QString kBrowseBackDisabledResource_    = ":/Resources/assets/browse_back_disabled.svg";
    const QString kBrowseForwardNormalResource_   = ":/Resources/assets/browse_fwd_normal.svg";
    const QString kBrowseForwardPressedResource_  = ":/Resources/assets/browse_fwd_pressed.svg";
    const QString kBrowseForwardDisabledresource_ = ":/Resources/assets/browse_fwd_disabled.svg";

    QHBoxLayout layout_;                 ///< The layout for the navigation bar.
    IconButton  browse_back_button_;     ///< The browse back navigation button.
    IconButton  browse_forward_button_;  ///< The browse forward navigation button.
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_NAVIGATION_BAR_H_
