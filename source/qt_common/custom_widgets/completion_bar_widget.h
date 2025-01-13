//=============================================================================
/// Copyright (c) 2016-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for a completion bar.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_COMPLETION_BAR_WIDGET_H_
#define QTCOMMON_CUSTOM_WIDGETS_COMPLETION_BAR_WIDGET_H_

#include <QColor>
#include <QWidget>

/// Support for the completion bar.
class CompletionBarWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(qreal percentage READ FillPercentage WRITE SetFillPercentage NOTIFY FillPercentageChanged);

public:
    /// Explicit constructor
    /// \param parent The parent of this widget
    explicit CompletionBarWidget(QWidget* parent = nullptr);

    /// Destructor
    virtual ~CompletionBarWidget();

    /// Provide a sizeHint for this widget.
    /// This particular widget does not have any specific dimensions that it needs,
    /// so it is really up to the layout to determine the size of this widget.
    /// Since this is the case, just return a simple default size here, and scale
    /// it according to the ScalingManager.
    /// \return The preferred size of the widget based on the current DPI scalefactor.
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

    /// The current fill percentage of the completion bar.
    /// \return The current filled percentage.
    qreal FillPercentage() const
    {
        return fill_percentage_;
    }

    /// Set how far to fill the bar, should be 1-100.
    /// \param percentage The percentage of the widget width to fill in.
    void SetFillPercentage(qreal percentage);

signals:
    /// Emitted when the fill percentage has changed.
    /// \param percentage The new fill percentage.
    void FillPercentageChanged(qreal percentage);

protected:
    /// Paint the completion bar.
    /// \param paint_event The painter event.
    virtual void paintEvent(QPaintEvent* paint_event) Q_DECL_OVERRIDE;

private:
    qreal fill_percentage_;  ///< Percentage of the bar that was filled in

    const QColor kEmptyColor_ = QColor(204, 204, 204);  ///< The default color for the empty portion of the widget.
    const QColor kFillColor_  = QColor(0, 118, 215);    ///< The default color for the filled portion of the widget.

    const int kDefaultWidth_  = 350;  ///< Default width of the widget.
    const int kDefaultHeight_ = 20;   ///< Default height of the widget.
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_COMPLETION_BAR_WIDGET_H_
