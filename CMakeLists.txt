cmake_minimum_required(VERSION 3.24)
project(QtCommon)

# Define version information
set(QTCOMMON_MAJOR_VERSION 3)
set(QTCOMMON_MINOR_VERSION 14)
if (NOT QTCOMMON_PATCH_NUMBER)
    set(QTCOMMON_PATCH_NUMBER 0)
endif ()
if (NOT QTCOMMON_BUILD_NUMBER)
    set(QTCOMMON_BUILD_NUMBER 0)
endif ()
string(TIMESTAMP DATE "\"%m/%d/%Y\"")
string(TIMESTAMP YEAR "\"%Y\"")

# Check if top-level project before attempting to query Qt dependencies
# This allows for consuming projects to override the Qt version used such as using
# Qt6 instead of Qt5
if (PROJECT_IS_TOP_LEVEL)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
    include(dev_tools)
    include(devtools_qt_helper)
endif ()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Buildinfo.properties.in" "${CMAKE_CURRENT_SOURCE_DIR}/Buildinfo.properties")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/source/version.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/source/version.h")

# Custom Widgets
add_subdirectory(source/qt_common/custom_widgets)

# Utilities
add_subdirectory(source/qt_common/utils)

# Packaging
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_GROUPING IGNORE)

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(CPACK_ARCHIVE_GUITEST_FILE_NAME "QtCommon_GuiTest_Debug-${QTCOMMON_MAJOR_VERSION}.${QTCOMMON_MINOR_VERSION}.${QTCOMMON_PATCH_NUMBER}.${QTCOMMON_BUILD_NUMBER}")
else ()
    set(CPACK_ARCHIVE_GUITEST_FILE_NAME "QtCommon_GuiTest_${QTCOMMON_MAJOR_VERSION}.${QTCOMMON_MINOR_VERSION}.${QTCOMMON_PATCH_NUMBER}.${QTCOMMON_BUILD_NUMBER}")
endif ()
