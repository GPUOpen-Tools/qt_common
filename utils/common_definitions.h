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
static const float kButtonFontPointSize  = 9.75f;

/// Default font pixel size for buttons.
static const int   kButtonFontPixelSize  = kButtonFontPointSize * kPointsToPixels;

/// Default check box width in pixels.
static const int   kDefaultCheckBoxWidth = 10;

/// Default ruler height in pixels.
static const int   kDefaultRulerHeight   = 50;

/// Link button stylesheet.
const static QString kLinkButtonStylesheet(
    "QPushButton"
    "{"
    "color: rgb(0, 0, 255);"
    "border: none;"
    "text-align: left"
    "}"

    "QPushButton:hover"
    "{"
    "color: rgb(255, 128, 0);"
    "}");

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

enum TimeUnitType
{
    kTimeUnitTypeMin,                       ///< The minimum value of the TimeUnitType enums
    kTimeUnitTypeClk = kTimeUnitTypeMin,    ///< Hardware clocks
    kTimeUnitTypeNanosecond,                ///< Nanoseconds (1 / 1,000,000,000 seconds)
    kTimeUnitTypeMicrosecond,               ///< Microseconds (1 / 1,000,000 seconds)
    kTimeUnitTypeMillisecond,               ///< Milliseconds (1 / 1,000 seconds)
    kTimeUnitTypeSecond,                    ///< Seconds
    kTimeUnitTypeMinute,                    ///< Minutes

    kTimeUnitTypeMax,                       ///< The maximum value of the TimeUnitType enums (not a valid time unit)
};

#endif  // QTCOMMON_UTILS_COMMON_DEFINITIONS_H_
