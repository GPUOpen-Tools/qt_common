cmake_minimum_required (VERSION 3.24)

# Build Qt .ui MOC files for the library.
set(CMAKE_AUTOMOC ON)

# Add all header and source files within the directory to the library.
file (GLOB CPP_INC
    "shared_isa_dictionary.h"
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
    "shared_isa_dictionary.cpp"
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
add_library(QtUtils STATIC ${CPP_SRC} ${CPP_INC})

# Set binary suffix
target_link_libraries(QtUtils Qt::Widgets)

target_include_directories(QtUtils PUBLIC ${PROJECT_SOURCE_DIR}/source PRIVATE . ../custom_widgets)

devtools_target_options(QtUtils)