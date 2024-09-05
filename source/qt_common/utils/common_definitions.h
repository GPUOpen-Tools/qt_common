//=============================================================================
/// Copyright (c) 2017-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  A set of common definitions used across multiple QtCommon components.
//=============================================================================

#ifndef QTCOMMON_UTILS_COMMON_DEFINITIONS_H_
#define QTCOMMON_UTILS_COMMON_DEFINITIONS_H_

/// Conversion factors between font points and pixels.
static const float kPointsToPixels = 1.33f;

/// Conversion factors between pixels and font points.
static const float kPixelsToPoints = 0.75f;

/// Default font point size for buttons.
static const float kButtonFontPointSize = 9.75f;

/// Default font pixel size for buttons.
static const int kButtonFontPixelSize = kButtonFontPointSize * kPointsToPixels;

/// Default check box width in pixels.
static const int kDefaultCheckBoxWidth = 10;

/// Default ruler height in pixels.
static const int kDefaultRulerHeight = 50;

/// Dark theme link button stylesheet.
const static QString kDarkLinkButtonStylesheet(
    "QPushButton"
    "{"
    "background-color: none;"
    "color: rgb(42, 130, 218);"
    "border: none;"
    "text-align: left"
    "}"

    "QPushButton:hover"
    "{"
    "background-color: none;"
    "color: rgb(255, 128, 0);"
    "}");

/// Light theme link button stylesheet.
const static QString kLinkButtonStylesheet(
    "QPushButton"
    "{"
    "background-color: none;"
    "color: rgb(0, 0, 255);"
    "border: none;"
    "text-align: left"
    "}"

    "QPushButton:hover"
    "{"
    "background-color: none;"
    "color: rgb(255, 128, 0);"
    "}");

#ifdef Q_OS_WIN

/// Value to set Windows to use dark mode colors for the window title bar.
static const int kUseDarkModeColors = 26;

#endif

struct RecentFileData
{
    QString path;                ///< Full path to the file
    QString keywords;            ///< Keywords designated to this trace
    QString api;                 ///< DX12 or Vulkan
    QString created;             ///< When was this trace created?
    QString accessed;            ///< When was this trace read?
    QString events;              ///< How many events in this trace?
    QString device_id;           ///< The GPU used to collect this trace
    QString device_revision_id;  ///< The GPU's revision ID
    QString device_string;       ///< The device string as reported by the driver
};

/// Struct to keep track of colors for custom painting.
struct DefaultThemeColors
{
    QColor graphics_scene_text_color;           ///< Default text color.
    QColor link_text_color;                     ///< Default link text color.
    QColor arrow_icon_border_color;             ///< Default border color for the arow icon combo box.
    QColor column_separator_color;              ///< Default column separator color for the shared isa tree view.
    QColor isa_background_row_color;            ///< Default secondary background row color in the shared isa viewer.
    QColor isa_search_match_row_color;          ///< Default search match row color in the shared isa viewer.
    QColor quick_link_button_background_color;  ///< Background color of the quick links button for light theme.
    QColor ruler_marker_color;                  ///< Marker color for the ruler widget.
    QColor ruler_edge_color;                    ///< Edge color for the ruler widget.
    QColor ruler_background_color;              ///< Background color for the ruler widget.
    QColor row_selected_color;                  ///< Background color for a row that has been selected in a table or tree.
    QColor window_background_color;             ///< Background color for for the window.
    QColor graphics_scene_background_color;     ///< Background color for for the graphics scenes.

    QString link_button_style_sheet;  ///< Default link_button style sheet.
};

enum TimeUnitType
{
    kTimeUnitTypeMin,                     ///< The minimum value of the TimeUnitType enums
    kTimeUnitTypeClk = kTimeUnitTypeMin,  ///< Hardware clocks
    kTimeUnitTypeNanosecond,              ///< Nanoseconds (1 / 1,000,000,000 seconds)
    kTimeUnitTypeMicrosecond,             ///< Microseconds (1 / 1,000,000 seconds)
    kTimeUnitTypeMillisecond,             ///< Milliseconds (1 / 1,000 seconds)
    kTimeUnitTypeSecond,                  ///< Seconds
    kTimeUnitTypeMinute,                  ///< Minutes
    kTimeUnitTypeHour,                    ///< Hours

    kTimeUnitTypeMax,  ///< The maximum value of the TimeUnitType enums (not a valid time unit)
};

/// \brief Enum for all color themes
enum ColorThemeType
{
    kColorThemeTypeLight = 0,  ///< Light theme.
    kColorThemeTypeDark  = 1,  ///< Dark theme.

    kColorThemeTypeCount  ///< Number of theme types.
};

namespace QtCommon::QtUtils
{
    /// @brief Defines typesafe color options that map to ColorThemeType
    enum class ColorThemeOption : uint8_t
    {
        kLight = kColorThemeTypeLight,
        kDark  = kColorThemeTypeDark,
        kSystem
    };
}

#endif  // QTCOMMON_UTILS_COMMON_DEFINITIONS_H_
