//=============================================================================
/// Copyright (c) 2017-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Header for a file loading widget
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_FILE_LOADING_WIDGET_H_
#define QTCOMMON_CUSTOM_WIDGETS_FILE_LOADING_WIDGET_H_

#include <QMap>
#include <QTimer>
#include <QWidget>

class AnimatedBars;

/// Class to handle the file loading animation.
class FileLoadingWidget : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    /// \param parent The animation widget's parent.
    FileLoadingWidget(QWidget* parent = nullptr);

    /// Virtual destructor.
    virtual ~FileLoadingWidget();

    /// Overridden sizeHint that returns a scaled default size.
    /// This really is just a hint, because this widget can resize to fit whatever
    /// space it is provided from the layout.
    /// \return The desired size of the widget.
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

public slots:
    /// Updates the animation.
    void Animate();

protected:
    /// Overridden paintEvent method.
    /// \param event The paint event object
    virtual void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

    /// Overridden resizeEvent handler.
    /// Recreates the animated bars with the newly resized dimensions.
    /// \param event The resize event.
    virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

private:
    const int kAnimationTimer_       = 33;  ///< Animation update rate, in ms
    const int kBarHorizontalSpacing_ = 5;   ///< The spacing between bars, in pixels
    const int kNumBars_              = 5;   ///< The number of bars in the animation

    QTimer*       animation_timer_;  ///< Timer to update widget
    AnimatedBars* animated_bars_;    ///< Class containing the animating bars
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_FILE_LOADING_WIDGET_H_
