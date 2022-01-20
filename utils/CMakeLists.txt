cmake_minimum_required (VERSION 3.5.1)
project (QtUtils)

set (QTUTIL_VERSION_MAJOR 1)
set (QTUTIL_VERSION_MINOR 0)

# Build Qt .ui MOC files for the library.
set(CMAKE_AUTOMOC ON)

# Add all header and source files within the directory to the library.
file (GLOB CPP_INC
    "color_generator.h"
    "color_palette.h"
    "common_definitions.h"
    "model_view_mapper.h"
    "qt_util.h"
    "restore_cursor_position.h"
    "scaling_manager.h"
    "zoom_icon_manager.h"
    "zoom_icon_group_manager.h"
    "locale_setting.h"
)

# Add all source files found within this directory.
file (GLOB CPP_SRC
    "color_generator.cpp"
    "color_palette.cpp"
    "model_view_mapper.cpp"
    "qt_util.cpp"
    "scaling_manager.cpp"
    "zoom_icon_manager.cpp"
    "zoom_icon_group_manager.cpp"
    "locale_setting.cpp"
)

# Pick up the source files that are relevant to the platform
add_library(${PROJECT_NAME} STATIC ${CPP_SRC} ${CPP_INC})

# Find Qt libraries to resolve dependencies.
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Set binary suffix
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
