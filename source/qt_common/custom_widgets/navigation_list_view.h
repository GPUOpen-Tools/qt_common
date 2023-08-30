//=============================================================================
/// Copyright (c) 2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header for a navigation list view
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_NAVIGATION_LIST_VIEW_H_
#define QTCOMMON_CUSTOM_WIDGETS_NAVIGATION_LIST_VIEW_H_

#include <QListView>

class NavigationListView : public QListView
{
    Q_OBJECT

public:
    /// Constructor.
    /// \param parent The view's parent.
    NavigationListView(QWidget* parent = nullptr);

    /// Destructor
    virtual ~NavigationListView();

    /// Handle mouse movement
    /// \param event The mouse event.
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

    /// Return a proper sizeHint based on the size of the items in the list.
    /// \return the size necessary to display the entire list.
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

private slots:
    /// Respond to changes in scaling factor by resizing the font used in the ListWidget
    /// and then updating the geometry.
    void OnScaleFactorChanged();

private:
    /// Default width to use as the sizeHint if there are no items in the list.
    const uint32_t kDefaultWidth_ = 200;

    /// Default padding to add to navigation list width
    const double kWidthPaddingFactor_ = 1.5;

    /// Custom stylesheet
    const QString kMainListWidgetStylesheet_ = QString(
        "QListView"
        "{"
        "background: rgb(240, 240, 240);"
        "border-right: 2px groove gray;"
        "outline: none;"
        "border: none;"
        "}"

        "QListView::item:hover"
        "{"
        "background: rgb(214, 214, 214);"
        "}"

        "QListView::item:selected"
        "{"
        "background: rgb(143, 193, 231);"
        "}"

        "QListView::item"
        "{"
        "border-bottom: none;"
        "padding: 7px;"
        "color: rgb(51, 51, 51);"
        "}");

    /// Calculates the width of the widest item in the list.
    int GetListWidgetWidth() const;

    /// Calculates the total height of all items in the list.
    int GetListWidgetHeight() const;
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_NAVIGATION_LIST_VIEW_H_
