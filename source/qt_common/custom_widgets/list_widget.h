//=============================================================================
/// Copyright (c) 2017-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Definition of a custom ListWidget that handles loss of focus.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_LIST_WIDGET_H_
#define QTCOMMON_CUSTOM_WIDGETS_LIST_WIDGET_H_

#include <QListWidget>

class ArrowIconComboBox;

/// Class to implement custom list widget behavior. Monitor the FocusChanged signal
/// and hide itself when losing focus
class ListWidget : public QListWidget
{
    Q_OBJECT

    friend class ArrowIconComboBox;

public:
    /// Explicit constructor.
    /// \param parent Pointer to the parent widget
    explicit ListWidget(QWidget* parent = nullptr);

    /// Explicit constructor.
    /// \param parent Pointer to the parent widget.
    /// \param button The associated ArrowIconWidget.
    /// \param hide Indicates if the list widget should be hidden initially.
    explicit ListWidget(QWidget* parent, ArrowIconComboBox* button, bool hide);

    /// Destructor.
    virtual ~ListWidget();

    /// Allow the user to set the parent.
    /// \param parent The parent widget
    void SetParent(QWidget* parent);

    /// Set whether the list box should go above the button (for list boxes at the
    /// bottom of the screen)
    /// \param list_above_button If true, draw the list box above the button. Otherwise
    /// draw if below (default)
    void SetListAboveButton(bool list_above_button);

    /// Add a new list item.
    /// \param label The string to be added to the list.
    void AddItem(const QString& label);

    /// When the ListWidget is shown, reposition it relative to the parent button
    /// so that it is initially painted in the correct location.
    /// \param event The Qt show event
    virtual void showEvent(QShowEvent* event) Q_DECL_OVERRIDE;

    /// Implementation of the paintEvent method for this class.
    /// \param event The Qt paint event
    virtual void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

    /// Implementation of the keyPressEvent method for this class.
    /// \param event The Qt key event
    virtual void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

    /// Overridden focusInEvent.
    /// \param event The focus event.
    virtual void focusInEvent(QFocusEvent* event) Q_DECL_OVERRIDE;

    /// Overridden focusOutEvent.
    /// \param event The focus event.
    virtual void focusOutEvent(QFocusEvent* event) Q_DECL_OVERRIDE;

    /// Overridden sizeHint that accounts for all the items in the list.
    /// \return The preferred size for this list widget.
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

    /// Update the size of the list widget based on its content.
    /// This is useful for when the ListWidget is a "pop-up" connected to the
    /// ArrowIconComboBox.
    void UpdateSize();

    /// Calculate the height of a list widget
    /// \return The height of the list widget
    int GetListWidgetHeight() const;

    /// Calculate the width of a list widget
    /// \return The width of the list widget
    int GetListWidgetWidth() const;

    /// Add a check box item to the list widget.
    /// \param text The text of the check box.
    /// \param list_widget The list widget to add the check box to.
    /// \param clicked_callback_slot Slot to call when the checkbox is clicked.
    /// \param widget The widget that the clicked_callback_slot is part of.
    /// \param list_name The name of list widget for verification.
    /// \param check_box_name The name to give the check box.
    static void AddListWidgetCheckboxItem(const QString&            text,
                                          ListWidget*&              list_widget,
                                          std::function<void(bool)> clicked_callback_slot,
                                          QWidget*                  widget,
                                          const QString&            list_name,
                                          const QString&            check_box_name);

    /// Update the "All" checkbox option.
    /// \param list_widget The list widget to update.
    static void UpdateAllCheckbox(ListWidget*& list_widget);

    /// Update the checked state of the checkboxes.
    /// \param list_widget The list widget to update.
    /// \param column_visibility An array containing the new checked states of the checkboxes.
    static void SetColumnVisibilityCheckboxes(ListWidget*& list_widget, const std::vector<bool>& column_visibility);

    /// Update the checked state of the checkboxes.
    /// \param list_widget The list widget to update.
    static std::vector<bool> GetColumnVisibilityCheckboxes(const ListWidget* list_widget);

signals:
    /// Emitted when the list widget gets focus.
    void FocusInEvent();

    /// Emitted when the list widget loses focus.
    void FocusOutEvent();

protected:
    ArrowIconComboBox* button_;  ///< The push button associated with this list box.

private slots:
    /// Slot to handle what happens when the list box loses focus
    /// \param old Pointer to the widget losing focus
    /// \param now Pointer to the widget getting focus
    void FocusChanged(QWidget* old, QWidget* now);

    // Update the size of the ListWidget when the scale factor is changed.
    void OnScaleFactorChanged();

private:
    /// Repositions the list widget either above or below the button that it is
    /// associated with.
    void RepositionListWidget();

    bool     hide_;                    ///< The hide flag to hide the widget on focusChanged event.
    QWidget* parent_;                  ///< The parent object.
    bool     show_list_above_button_;  ///< Show the list widget above the button (true) or below (false).
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_LIST_WIDGET_H_
