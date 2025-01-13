//=============================================================================
/// Copyright (c) 2020-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief A QScrollArea derived class that increases the width when horizontal
/// scrollbar is turned off or increases the height if vertical scrollbar
/// is turned off in order to accommodate viewing the entire contents of the
/// scroll area horizontally or vertically.  The size of the control will grow
/// larger, but will not shrink if the size of the contents widget is reduced.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_EXPANDING_SCROLL_AREA_H_
#define QTCOMMON_CUSTOM_WIDGETS_EXPANDING_SCROLL_AREA_H_

#include <QScrollArea>

/// Customized scrollarea with expanding width and/or height.
class ExpandingScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    /// Constructor.
    /// \param parent The widget's parent.
    ExpandingScrollArea(QWidget* parent);

    /// Destructor.
    virtual ~ExpandingScrollArea();

    /// Reset the vertical and horizontal limits of the scroll area.
    void ResetGeometry();

    /// Return a proper sizeHint based on the size of the scroll area's content widget.
    /// \return the dimensions of the scroll area.
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

    /// Return a proper minimum sizeHint based on the size of the scroll area's content widget.
    /// \return the dimensions of the scroll area (same as sizeHint).
    virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;

private slots:
    /// Callback for when the scale factor has changed.
    void OnScaleFactorChanged();

protected:
    /// Adjusts the size of the scroll area width so that it is large
    /// enough to view the scroll area contents horizontally when the
    /// horizontal scrollbar is turned off or vertically when the vertical
    /// scrollbar is turned off.
    /// \param object A pointer to the object that triggered the event.
    /// \param event A pointer to the resize event.
    virtual bool eventFilter(QObject* destination_object, QEvent* event) Q_DECL_OVERRIDE;

private:
    QSize size_hint_;  ///< The maintained size hint for the scroll area.
};
#endif  // QTCOMMON_CUSTOM_WIDGETS_EXPANDING_SCROLL_AREA_H_
