//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  A set of common definitions used across multiple QtCommon components.
//=============================================================================

#ifndef _COMMON_DEFINITIONS_H_
#define _COMMON_DEFINITIONS_H_

#ifdef __RGA
static const int s_BUTTON_PIXEL_FONT_SIZE = 11;
#else
static const int s_BUTTON_PIXEL_FONT_SIZE = 13;
#endif
static const int s_CHECK_BOX_WIDTH = 10;
static const int s_RULER_HEIGHT = 50;

static const QString s_LIST_WIDGET_STYLE = QString("font-size: %1px; text-align:center;");

static const QString gs_QT_STRING_OFF = "OFF";
static const QString gs_QT_STRING_ON = "ON";

// Link button stylesheet
const static QString gs_LINK_BUTTON_STYLESHEET
(
    "QPushButton"
    "{"
    "color: rgb(0, 0, 255);"
    "border: none;"
    "text-align: left"
    "}"

    "QPushButton:hover"
    "{"
    "color: rgb(255, 128, 0);"
    "}"
);

static const QString gs_CUSTOM_SLIDER_STYLESHEET
(
    "QSlider::groove:horizontal {"
    "height: 2px;"
    "background-color : #AFAFAF;"
    "margin: 2px 0;"
    "}"

    "QSlider::handle:horizontal {"
    "background-color : #0074D6;"
    "width: 6px;"
    "height:15px;"
    "margin: -15px 0;"
    "}"

    "QSlider::handle:horizontal:hover {"
    "background-color : black;"
    "width: 6px;"
    "height:15px;"
    "margin: -15px 0"
    "}"

    "QSlider::handle:disabled {"
    "background-color : grey;"
    "width: 6px;"
    "height:15px;"
    "margin: -15px 0"
    "}"
);

struct RecentFileData
{
    QString path;               ///< Full path to the file
    QString keywords;           ///< Keywords designated to this trace
    QString api;                ///< DX12 or Vulkan
    QString created;            ///< When was this trace created?
    QString accessed;           ///< When was this trace read?
    QString events;             ///< How many events in this trace?
    QString deviceId;           ///< The GPU used to collect this trace
    QString deviceRevisionId;   ///< The GPU's revision ID
    QString deviceString;       ///< The device string as reported by the driver
};

enum TimeUnitType
{
    TIME_UNIT_MIN,
    TIME_UNIT_TYPE_CLK = TIME_UNIT_MIN,
    TIME_UNIT_TYPE_NS,
    TIME_UNIT_TYPE_US,
    TIME_UNIT_TYPE_MS,

    TIME_UNIT_MAX,
};

#endif // _COMMON_DEFINITIONS_H_