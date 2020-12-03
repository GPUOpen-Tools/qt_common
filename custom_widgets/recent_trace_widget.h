//=============================================================================
/// Copyright (c) 2019-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief A widget which encapsulates a single recent trace selection. This
/// class can be used by itself or classes can derive from it if they require
/// additional information for each recent trace file.
/// Examples of this can be seen in the "Recent profiles" pane in RGP and RMV.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_RECENT_TRACE_WIDGET_H_
#define QTCOMMON_CUSTOM_WIDGETS_RECENT_TRACE_WIDGET_H_

#include <QWidget>
#include <QSignalMapper>
#include <QVBoxLayout>

#include "recent_trace_mini_widget.h"
#include "scaled_label.h"
#include "scaled_push_button.h"

struct RecentFileData;

/// Widget that represents a single trace in the recent traces pane
class RecentTraceWidget : public RecentTraceMiniWidget
{
    Q_OBJECT

public:
    /// Constructor
    /// \param parent The RecentTraceWidget's parent.
    explicit RecentTraceWidget(QWidget* parent = nullptr);

    /// Destructor
    virtual ~RecentTraceWidget();

    /// overridden Qt changeEvent
    /// \param event The change event that occurred.
    virtual void changeEvent(QEvent* event);

    /// Set the trace information for this widget from an RGPRecentFileData object.
    /// Deprecated, use SetRecentFileData instead.
    /// \param file The file this widget is going to show trace data from.
    virtual void Initialize(const RecentFileData& file);

    /// Set the file information for this widget from a RecentFileData object.
    /// \param file The file to display information about.
    virtual void SetRecentFileData(const RecentFileData& file);

    /// Set the label string used for the "Open file location" button.
    /// \param label The text string to use for the button.
    void SetOpenFileLocationText(const QString& label);

    /// Retrieve the path of the trace file associated with this widget.
    /// \returns The path string.
    virtual QString GetPath() const;

public slots:
    /// Open the file browser for this trace.
    /// \param checked Unused
    void OpenFileLocation(bool checked);

signals:
    /// Signal emitted when the delete link is clicked.
    /// \param path The path to the trace file
    void clickedDelete(QString path);

    /// Signal emitted if the OpenFileLocation() method fails due
    /// to an invalid file or folder.
    /// \param path The path that failed to open.
    void OpenFileLocationFailed(const QString path);

protected:
    /// Derived classes will implement this function to add custom widgets
    /// to the recent trace widget if they require them. Custon widgets will
    /// need adding to the widget_layout_.
    virtual void AddCustomWidgets();

    /// Get the time that the provided file was last accessed, as a string.
    /// \param file The file data
    QString GetLastAccessedTime(const RecentFileData& file);

    /// Set up the fonts.
    /// \param font_size The size of the larger font used for the path name. The
    /// details font size is derived from this.
    void SetupFonts(qreal font_size);

    ScaledLabel* access_label_;  ///< Label with access date text

private:
    QHBoxLayout*      option_buttons_layout_;      ///< The layout that holds the option buttons.
    QSignalMapper*    delete_signal_mapper_;       ///< Delete button signal mapper
    ScaledPushButton* delete_button_;              ///< Button to delete this trace file
    ScaledPushButton* open_file_location_button_;  ///< Button to open file browser for this trace file.
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_RECENT_TRACE_WIDGET_H_
