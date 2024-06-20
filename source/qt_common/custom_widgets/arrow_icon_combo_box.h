//=============================================================================
/// Copyright (c) 2018-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header for our down arrow combobox
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_ARROW_ICON_COMBO_BOX_H_
#define QTCOMMON_CUSTOM_WIDGETS_ARROW_ICON_COMBO_BOX_H_

#include <memory>

#include <QCheckBox>
#include <QLabel>
#include <QListWidgetItem>
#include <QPushButton>

#include "qt_common/custom_widgets/list_widget.h"

/// struct for highlight data
struct StringHighlightData
{
    /// The start location for highlighting
    int start_location;

    /// The end location for highlighting
    int end_location;

    /// The highlighting color
    QColor highlight_color;

    /// The highlight string
    std::string highlight_string;
};

/// Class that implements the up/down arrow combobox widgets
class ArrowIconComboBox : public QPushButton
{
    Q_OBJECT

    Q_PROPERTY(QColor fontColor READ GetFontColor WRITE SetFontColor)
    Q_PROPERTY(int currentItem READ CurrentRow WRITE SetSelectedRow)

    friend class ListWidget;

public:
    /// Constructor for ArrowIconComboBox.
    /// \param parent The parent widget
    explicit ArrowIconComboBox(QWidget* parent = nullptr);

    /// Destructor
    virtual ~ArrowIconComboBox();

    /// Provide a sizeHint which includes the arrow and the text width.
    /// \return The desired size of the widget
    QSize sizeHint() const Q_DECL_OVERRIDE;

    /// Init the combobox to allow single-selection.
    ///
    /// Single-select comboboxes will close immediately after a selection is made, cannot have checkboxes, and can optionally retain the default text
    /// or insert a prefix text into the combobox when an option is selected.
    /// \param main_parent The main pane that this combobox is on (since the list widget needs to appear on-top of the pane).
    /// \param default_text The default text to display in the widget before an item is selected.
    /// \param retain_default_text Keep the default text when an option is selected (rather than changing the text to reflect the selected option).
    /// \param prefix_text If retain_default_text is false, this optional text will be prepended to the selected item to form the main text in the combobox.
    void InitSingleSelect(QWidget* main_parent, const QString& default_text, bool retain_default_text, const QString& prefix_text = "");

    /// Init the combobox to allow multi-selection.
    ///
    /// Multi-select comboboxes will stay open after a selection is made so the user can select additional options; it will close when a different widget gets focus.
    /// Multi-select comboboxes can only have checkboxes, and the default text will always remain in the combobox. There can be an "All" checkbox option added with
    /// custom text that will be automatically handled to check / uncheck all the other options.
    /// \param main_parent The main pane that this combobox is on (since the list widget needs to appear on-top of the pane).
    /// \param default_text The default text to display in the widget before an item is selected.
    void InitMultiSelect(QWidget* main_parent, const QString& default_text);

    /// Set whether the list box should go above the button (for list boxes at the bottom of the screen).
    /// \param list_above_button If true, draw the list box above the button. Otherwise draw if below (default).
    void SetListAboveButton(bool list_above_button);

    /// Add a new list item.
    /// \param item The list widget item to add.
    void AddItem(QListWidgetItem* item);

    /// Add a new list item.
    /// Labels will be added for single-select comboboxes,
    /// and checkboxes will be added for multi-select comboboxes.
    /// \param item_text The string to be added to the list.
    /// \param user_data User data for item, assigned to the UserRole.
    void AddItem(const QString& item_text, const QVariant& user_data = QVariant());

    /// Add a new checkbox list item to a multi-select combobox.
    /// \param item_text The string to be added to the list.
    /// \param user_data the user data for item, assigned to the UserRole.
    /// \param checked Sets the initial checked state of the checkbox.
    /// \param is_all_option true if item should be used as 'All' checkbox (can only be true for one checkbox item).
    /// \return nullptr if this is a single-select combobox; otherwise the checkbox created.
    QCheckBox* AddCheckboxItem(const QString& item_text, const QVariant& user_data, bool checked, bool is_all_option);

    /// Clear the list of all items.
    void ClearItems();

    /// Finds item with specified widget as the itemWidget.
    /// \param widget the item widget.
    /// \return item with widget as itemWidget.
    const QListWidgetItem* FindItem(const QWidget* widget) const;

    /// Finds item from specified index.
    /// \param list_index The index of the item to find.
    /// \return           The item with list_index as its index.
    QListWidgetItem* FindItem(const int list_index) const;

    /// Gets data stored in item at specified index
    /// \param index the item index
    /// \param role Data role defaults to Qt::UserRole.
    QVariant ItemData(int index, int role = Qt::UserRole) const;

    /// Remove all list items with the given string.
    /// \param item_string The string to be removed from the list.
    void RemoveItem(const QString& item_string);

    /// Remove a single list item at the given index.
    /// \param item_index The index to be removed from the list.
    void RemoveItem(const int item_index);

    /// Is an item in the list checked
    /// \param list_index The index of the item in the list to test
    /// \return true if checked, false otherwise
    bool IsChecked(int list_index) const;

    /// Set the check state of an item in the combo box
    /// \param list_index The index of the item in the combo box
    /// \param checked The new state of the item checked state
    /// \return true if state changed, false if error
    bool SetChecked(int list_index, bool checked);

    /// Show/Hide an individual row in the list.
    /// \param list_index The index of the row.
    /// \param hidden     The state of the row.
    void SetRowHidden(int list_index, bool hidden);

    /// @Brief Disables/enables the item at the given index. If the index is invalid, does nothing.
    ///
    /// @param list_index The index of the item to be disabled/enabled.
    /// @param disabled   Whether the item should be disabled (true) or enabled (false).
    void SetDisabled(int list_index, bool disabled);

    /// @Brief Gets the index of the last checked item if there is one.
    ///        Helper function to aid in disabling the last checked item.
    ///
    /// @Return Returns -1 if there are multiple checked items and none are disabled.
    ///         If there is only 1 item checked, it returns the index of that item. If there
    ///         are multiple items checked, but 1 is disabled, it returns the index of that
    ///         item. This is to help in the situation where a second item was just checked
    ///         and the disabled item needs to be re-enabled. This function assumes only 1
    ///         item can be disabled at a time.
    int LastCheckedIndex();

    /// Update widget state
    void Update();

    /// Reset to starting state
    void ResetSelection();

    /// Get previous row index in list
    /// \pre The combobox must be initialized
    /// \return item list previous row index or current if at beginning
    int PrevRow() const;

    /// Get current row index in list
    /// \pre The combobox must be initialized
    /// \return item list current row index
    int CurrentRow() const;

    /// Get next row index in list
    /// \pre The combobox must be initialized
    /// \return item list next row index or current if at end of list
    int NextRow() const;

    /// Get the number of rows
    /// \return item list row count
    int RowCount() const;

    /// Set current row
    /// \param index the list index to select
    void SetSelectedRow(int index);

    /// @brief Clear any row that is currently shown as selected.
    void ClearSelectedRow();

    /// Specify a maximum height of the pop-up list.
    /// \param height new height
    void SetMaximumHeight(int height);

    /// Specify a fixed width to the pop-up list.
    /// \param width new width
    void SetFixedWidth(int width);

    /// Get the text of the currently selected item.
    /// \return The text of the currently selected item, not including the prefix text.
    QString SelectedText() const
    {
        return selected_text_;
    }

    /// Get the default text of the widget.
    /// \return The default widget text.
    QString DefaultText() const
    {
        return default_text_;
    }

    /// Indicates whether or not the "All" choice is selected.
    /// \return True if the "All" choice exists and is selected; false otherwise.
    bool IsAllChoiceSelected() const
    {
        return all_choice_selected_;
    }

    /// Retrieve the text string for a list widget item.
    /// \param list_item The item in the combobox to retrieve the text for.
    /// \return The text string for the item.
    QString ItemText(QListWidgetItem* list_item) const;

    /// Set the color of the widget.
    /// \param color Color of the widget
    void SetColor(const QColor& color);

    /// Set the font color of the widget.
    /// \param color Font color of the widget
    void SetFontColor(const QColor& color);

    /// Set the size of the encompassing rectangle.
    /// \param size The size of the encompassing rectangle.
    void SetSize(int size);

    /// Set the pen width of the arrow.
    /// \param width The pen width of the arrow.
    void SetPenWidth(int width);

    /// Set the boolean to indicate highlighting of substring.
    /// \param value The boolean to indicate if highlight requested.
    void SetHighLightSubString(bool value);

    /// Set substring data.
    /// \param string_highlight_data Data about the string to highlight.
    void SetHighLightSubStringData(QVector<StringHighlightData> string_highlight_data);

    /// Clear highlight substring data.
    void ClearHighLightSubStringData();

    /// Returns the current font color.
    /// \return The current font color.
    QColor GetFontColor() const
    {
        return font_color_;
    }

    /// Set the color of the widget border.
    /// \param color Color of the widget border
    void SetBorderColor(const QColor& color);

    /// Set the boolean to indicate if the border should be drawn.
    /// \param value The boolean to indicate if the border should be drawn
    void SetShowBorder(bool value);

    /// @brief Remove the event filter that was installed during initialization.
    void RemoveEventFilter();

    /// @brief Toggle the direction of the arrow next to the combo box.
    void ToggleDirection();

signals:
    /// Emitted when the list has been opened.
    void Opened();

    /// Emitted when the list has been closed.
    void Closed();

    /// Signal emitted when the ArrowIconWidget gets focus.
    void FocusInEvent();

    /// Signal emitted when the ArrowIconWidget loses focus.
    void FocusOutEvent();

    /// Emitted when the list selection changes.
    void SelectionChanged();

    /// Emitted when an item is selected
    /// \param item The item that has been selected
    void SelectedItem(QListWidgetItem* item);

    /// Emitted when the checked state of an item has changed.
    /// If you need to know which checkbox has changed, use CheckboxChanged instead.
    /// \param checkstate The new checked state of the item.
    void CheckboxStateChanged(bool checkstate);

    /// Emitted when the checked state of a checkbox has changed.
    /// \param checkbox The checkbox that has been changed.
    void CheckboxChanged(QCheckBox* checkbox);

protected:
    /// Sets an anchor point for item list
    ///
    /// This method sets an anchor point for the item list
    /// from an already known location.
    ///
    /// \param point The anchor point
    void SetAnchor(QPoint point);

    /// Cache the last user selection
    /// \param selection the selected text
    void SetSelectedText(const QString& selection);

    /// Helper function to update the "All" option of a multi-select combobox when one of the other options is checked.
    /// \param recently_checked The new checked state of a checkbox.
    void UpdateAllOption(bool recently_checked);

    /// Toggle the list when the mouse button is clicked
    /// \param event The mouse event
    virtual void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

    /// Event filter to make the list close when clicking outside of widget
    /// \param object the test object
    /// \param event the actual event
    /// \return true to filter out the event, false to allow it to be processed.
    virtual bool eventFilter(QObject* object, QEvent* event) Q_DECL_OVERRIDE;

    /// Overridden paintEvent method.
    /// \param event The paint event object
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

    /// Called when the ArrowIconWidget gets keyboard focus.
    /// \param event A QFocusEvent to inform how focus was obtained.
    void focusInEvent(QFocusEvent* event) Q_DECL_OVERRIDE;

    /// Called when the ArrowIconWidget loses keyboard focus.
    /// \param event A QFocusEvent to inform why focus was lost.
    void focusOutEvent(QFocusEvent* event) Q_DECL_OVERRIDE;

    /// Override the show event to invalidate the font metrics when first shown.
    /// \param show_event The QShowEvent.
    void showEvent(QShowEvent* show_event) Q_DECL_OVERRIDE;

private slots:
    /// Handle what happens when a list item is clicked
    /// \param item The clicked item
    void ListItemClicked(QListWidgetItem* item);

    /// Handle what happens when the "All" checkbox is clicked. Set the state of
    /// all checkboxes to match the "All" checkbox state.
    /// \param checked The state of the checkbox selected
    void AllCheckboxClicked(bool checked);

    /// Handle what happens when a checkbox is clicked. The state of the 'All' button will
    /// need updating depending on the state of all the other checkboxes.
    /// \param checked The state of the checkbox selected
    void OnCheckboxClicked(bool checked);

    /// Updates font size and arrow size on DPI scale changes.
    void OnScaleFactorChanged();

private:
    /// enum to specify up/down arrow
    enum Direction
    {
        DownArrow,
        UpArrow
    };

    /// Initializes the item list object.
    /// \param main_parent The main pane that this combobox is on (since the item list needs to appear on-top of the pane).
    void InitItemList(QWidget* main_parent);

    /// Open the list
    void OpenMenu();

    /// Close the list
    void CloseMenu();

    /// Add a new label list item to a multi-select combobox.
    /// \param item_text The string to be added to the list.
    /// \param user_data the user data for item, assigned to the UserRole.
    /// \return nullptr if this is a multi-select combobox; otherwise the label created.
    QLabel* AddLabelItem(const QString& item_text, const QVariant& user_data = QVariant());

    /// Create the vertices for the arrow.
    void CreateVertices();

    /// Disconnect the signal/slots associated with the "All" checkbox functionality
    void DisconnectSignals();

    /// Set the direction of the arrow.
    /// \param direction The direction of the arrow.
    void SetDirection(Direction direction);

    /// The number of vertices in the arrow.
    static const int kNumberOfVertices = 3;

    std::unique_ptr<ListWidget> item_list_;           ///< The list of items.
    QString                     default_text_;        ///< The default starting string.
    QString                     selected_text_;       ///< Track selected text.
    QString                     prefix_text_;         ///< The text that gets prefixed to the selected text.
    bool                        allow_multi_select_;  ///< Whether this guy supports multi-selection.
    bool show_list_above_button_;                     ///< Is the list to be displayed above the button (when the list widget is at the bottom of the screen).
    bool retain_default_text_;                        ///< Flag to retain default text when selection changes.
    bool all_choice_added_;                           ///< Indicates that an "All" checkbox has already been added to a multi-select combobox.
    bool all_choice_selected_;                        ///< Does this combo box have the "All" option selected.
    bool first_show_;                                 ///< Boolean to track the first time this widget is shown to invalidate the font metrics.

    int                          size_;                         ///< The size of the encompassing rect.
    QColor                       color_;                        ///< The color of the arrow's lines.
    QColor                       border_color_;                 ///< The color of the widget border, if border requested.
    bool                         use_fixed_border_color_;       ///< Whether the border color was set to a fixed color.
    bool                         show_border_;                  ///< The boolean to indicate if border requested.
    QColor                       font_color_;                   ///< The text font color.
    bool                         use_fixed_font_color_;         ///< Whether the font color was set to a fixed color.
    Direction                    direction_;                    ///< The direction of the arrow.
    int                          pen_width_;                    ///< The width of the pen.
    QPointF                      vertices_[kNumberOfVertices];  ///< The vertices of the arrow.
    bool                         highlight_substring_;          ///< The boolean to indicate if highlight requested.
    QVector<StringHighlightData> string_highlight_data_ = {};   ///< The vector to keep track of highlight strings.
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_ARROW_ICON_COMBO_BOX_H_
