//=============================================================================
// Copyright (c) 2023 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Header for a shared isa tree vertical scroll bar.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_VERTICAL_SCROLL_BAR_H_
#define QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_VERTICAL_SCROLL_BAR_H_

#include <QScrollBar>

#include <set>

/// @brief SharedIsaVerticalScrollBar is a scroll bar that custom paints the relative position
///        of hot spots and text search matches as red and purple rectangles, inside of the scroll bar.
class SharedIsaVerticalScrollBar final : public QScrollBar
{
    Q_OBJECT

public:
    /// @brief Constructor.
    ///
    /// @param [in] parent The parent widget.
    explicit SharedIsaVerticalScrollBar(QWidget* parent = nullptr);

    /// @brief Destructor.
    virtual ~SharedIsaVerticalScrollBar();

    /// @brief Set the line # of a hot spot to paint a red rectangle.
    ///
    /// @param [in] line_number The line # of the hot spot.
    void SetHotSpotLineNumber(int line_number);

    /// @brief Set the line #(s) of text search matches to paint a purple rectangle(s).
    ///
    /// @param [in] line_numbers The line #(s) of the text search matches.
    void SetSearchMatchLineNumbers(const std::set<int>& line_numbers);

protected:
    /// @brief Override paint to paint red hot spots and purple text search matches.
    ///
    /// @param [in] The paint event.
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

    int           hot_spot_line_number_;       ///< Line number of a hot spot.
    std::set<int> search_match_line_numbers_;  ///< Line number(s) of text search matches.
};

#endif  // QTCOMMON_CUSTOM_WIDGETS_SHARED_ISA_VERTICAL_SCROLL_BAR_H_
