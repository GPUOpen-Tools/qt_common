//=============================================================================
/// Copyright (c) 2018-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header for a navigation list widget
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_NAVIGATION_LIST_WIDGET_H_
#define QTCOMMON_CUSTOM_WIDGETS_NAVIGATION_LIST_WIDGET_H_

#include <QListWidget>
#include <QString>

/// Wavefront graphics view that is aware of resize and mouse events
class NavigationListWidget : public QListWidget
{
    Q_OBJECT

public:
    /// Constructor.
    /// \param parent The view's parent.
    NavigationListWidget(QWidget* parent = nullptr);

    /// Destructor
    virtual ~NavigationListWidget();

    /// Handle mouse movement
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

    /// Return a proper sizeHint based on the size of the items in the list.
    /// \return the size necessary to display the entire list.
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

    /// Set either thelight theme or dark theme stylsheet based on the current color theme.
    void SetStyleSheet();

private slots:
    /// Respond to changes in scaling factor by resizing the font used in the ListWidget
    /// and then updating the geometry.
    void OnScaleFactorChanged();

private:
    /// Default width to use as the sizeHint if there are no items in the list.
    const uint32_t kDefaultWidth_ = 200;

    /// Custom light theme stylesheet
    const QString kMainListWidgetStylesheet_ = QString(
        "QListWidget"
        "{"
        "background: rgb(240, 240, 240);"
        "border-right: 2px groove gray;"
        "outline: none;"
        "border: none;"
        "}"

        "QListWidget::item:hover"
        "{"
        "background: rgb(214, 214, 214);"
        "}"

        "QListWidget::item:selected"
        "{"
        "background: rgb(143, 193, 231);"
        "}"

        "QListWidget::item"
        "{"
        "border-bottom: none;"
        "padding: 7px;"
        "color: rgb(51, 51, 51);"
        "}");

    /// Custom dark theme stylesheet
    const QString kMainListWidgetDarkStylesheet_ = QString(
        "QListWidget"
        "{"
        "background: rgb(40, 40, 40);"
        "border-right: 2px groove gray;"
        "outline: none;"
        "border: none;"
        "}"

        "QListWidget::item:hover"
        "{"
        "background: rgb(70, 70, 70);"
        "}"

        "QListWidget::item:selected"
        "{"
        "background: rgb(100, 150, 200);"
        "}"

        "QListWidget::item"
        "{"
        "border-bottom: none;"
        "padding: 7px;"
        "color: rgb(255, 255, 255);"
        "}");

    /// Calculates the width of the widest item in the list.
    int GetListWidgetWidth() const;

    /// Calculates the total height of all items in the list.
    int GetListWidgetHeight() const;
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_NAVIGATION_LIST_WIDGET_H_
