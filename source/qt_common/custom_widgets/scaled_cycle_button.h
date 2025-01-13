//=============================================================================
/// Copyright (c) 2020-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Scaling cycle button that cycles through a set of options
/// each click.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SCALED_CYCLE_BUTTON_H_
#define QTCOMMON_CUSTOM_WIDGETS_SCALED_CYCLE_BUTTON_H_

#include <QPushButton>
#include <QStringList>

/// Wraps the QPushButton and fixes DPI scaling.
class ScaledCycleButton : public QPushButton
{
    Q_OBJECT

public:
    /// Default Constructor.
    /// \param parent the parent widget.
    ScaledCycleButton(QWidget* parent = nullptr);

    /// Constructor
    /// \param text_items The list if items that can be displayed on the button.
    /// \param parent the parent widget.
    ScaledCycleButton(const QStringList& text_list, QWidget* parent = nullptr);

    /// Destructor.
    virtual ~ScaledCycleButton();

    /// Initialize the button with a list of items that are cycled through
    /// each time the button is clicked.
    void Init(const QStringList& text_list);

    /// Overridden to return the scaled size hint for the ScaledCycleButton.
    /// The size is determined by the larges text item in the list.
    /// \return scaled size
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

    /// Retrieve the currently selected item index.
    /// \returns The zero based index of the item in the item_list currently selected.
    /// If the list of items is empty, -1 is returned.
    int CurrentItemIndex() const;

    /// Set the current index and update the button text.
    /// \param index The index number of the text item to select.
    /// \returns If the index is valid, true is returned.  Otherwise
    /// false is returned.
    bool SetCurrentItemIndex(int index);

private slots:

    /// Cycle to the next item in the list and update the button text.  When the last
    /// item in the list is reached, the button will wrap around back to the first item.
    /// \param enabled Unused.
    void NextItem(bool enabled);

signals:
    /// A Signal used to notify when the button has been clicked.  The index indicates
    /// the current item displayed in the button.  This signal should be connected to
    /// a slot instead of the QPushButton::clicked() signal.
    void Clicked(int index);

private:
    /// Indicates that the widget has been initialized if true.
    bool init_;

    /// A list of text items displayed on the button for each click.
    QStringList text_items_;

    /// The currently selected item.  Set to -1 if the list of items
    /// is empty.
    int current_item_;
};
#endif  // QTCOMMON_CUSTOM_WIDGETS_SCALED_CYCLE_BUTTON_H_
