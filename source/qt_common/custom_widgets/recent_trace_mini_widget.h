//=============================================================================
/// Copyright (c) 2017-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for a mini recent trace widget.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_RECENT_TRACE_MINI_WIDGET_H_
#define QTCOMMON_CUSTOM_WIDGETS_RECENT_TRACE_MINI_WIDGET_H_

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

#include "qt_common/utils/common_definitions.h"
#include "qt_common/custom_widgets/scaled_push_button.h"

/// Widget that represents a single trace in the recent traces pane
class RecentTraceMiniWidget : public QWidget
{
    Q_OBJECT

public:
    /// Explicit constructor.
    /// \param parent The color picker widget's parent.
    explicit RecentTraceMiniWidget(QWidget* parent = nullptr);

    /// Destructor.
    virtual ~RecentTraceMiniWidget();

    /// Set the trace information for this widget from a RecentFileData object.
    /// \param file The file this widget is going to show trace data from.
    void SetFile(const RecentFileData& file);

    /// Retrieve the path of the trace file associated with this widget.
    /// \returns The path string.
    virtual QString GetPath() const;

    /// Overridden sizeHint which returns the size of ScaledPushButton.
    /// \return The desired size of this widget.
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
    QVBoxLayout*      widget_layout_;  ///< This widget's layout containing all stuff
    ScaledPushButton* path_button_;    ///< Button with file path text

    /// Reimplemented the fontChange event to pass the font
    /// on to the path button.
    virtual void changeEvent(QEvent* event) Q_DECL_OVERRIDE;

signals:
    void clicked(QString);
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_RECENT_TRACE_MINI_WIDGET_H_
