cmake_minimum_required (VERSION 3.5.1)
project (QtCustomWidgets)

set (QTCUSTOMWIDGETS_VERSION_MAJOR 1)
set (QTCUSTOMWIDGETS_VERSION_MINOR 0)

# Build Qt .ui MOC files for the library.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt libraries to resolve dependencies.
find_package(Qt5 COMPONENTS Widgets Gui REQUIRED)

# Add all header and source files within the directory to the library.
file (GLOB CPP_INC
    "arrow_icon_combo_box.h"
    "check_box_widget.h"
    "colored_legend_graphics_view.h"
    "colored_legend_scene.h"
    "completion_bar_widget.h"
    "donut_pie_widget.h"
    "double_slider_widget.h"
    "elided_line_label.h"
    "expanding_scroll_area.h"
    "file_loading_widget.h"
    "graphics_scene.h"
    "graphics_view.h"
    "icon_button.h"
    "list_widget.h"
    "message_overlay.h"
    "message_overlay_container.h"
    "navigation_bar.h"
    "navigation_list_widget.h"
    "navigation_list_view.h"
    "navigation_list_model.h"
    "quick_link_button_widget.h"
    "recent_trace_mini_widget.h"
    "recent_trace_widget.h"
    "ruler_widget.h"
    "scaled_check_box.h"
    "scaled_combo_box.h"
    "scaled_font_combo_box.h"
    "scaled_cycle_button.h"
    "scaled_group_box.h"
    "scaled_header_view.h"
    "scaled_label.h"
    "scaled_line_edit.h"
    "scaled_menu_bar.h"
    "scaled_push_button.h"
    "scaled_spin_box.h"
    "scaled_double_spin_box.h"
    "scaled_tab_widget.h"
    "scaled_table_view.h"
    "scaled_tree_view.h"
    "shared_isa_item_delegate.h"
    "shared_isa_item_model.h"
    "shared_isa_branch_label_navigation_widget.h"
    "shared_isa_proxy_model.h"
    "shared_isa_tree_view.h"
    "shared_isa_widget.h"
    "tab_bar.h"
    "tree_view.h"
    "tab_widget.h"
    "text_search_widget.h"
    "timeline_view.h"
)

# Add any .ui  files
file (GLOB UI_SRC
    "message_overlay.ui"
)

# Add all source files found within this directory.
file (GLOB CPP_SRC
    "arrow_icon_combo_box.cpp"
    "check_box_widget.cpp"
    "colored_legend_graphics_view.cpp"
    "colored_legend_scene.cpp"
    "completion_bar_widget.cpp"
    "donut_pie_widget.cpp"
    "double_slider_widget.cpp"
    "elided_line_label.cpp"
    "expanding_scroll_area.cpp"
    "file_loading_widget.cpp"
    "graphics_scene.cpp"
    "graphics_view.cpp"
    "icon_button.cpp"
    "list_widget.cpp"
    "message_overlay.cpp"
    "message_overlay_container.cpp"
    "navigation_bar.cpp"
    "navigation_list_widget.cpp"
    "navigation_list_view.cpp"
    "navigation_list_model.cpp"
    "quick_link_button_widget.cpp"
    "recent_trace_mini_widget.cpp"
    "recent_trace_widget.cpp"
    "ruler_widget.cpp"
    "scaled_check_box.cpp"
    "scaled_combo_box.cpp"
    "scaled_font_combo_box.cpp"
    "scaled_cycle_button.cpp"
    "scaled_group_box.cpp"
    "scaled_header_view.cpp"
    "scaled_label.cpp"
    "scaled_line_edit.cpp"
    "scaled_menu_bar.cpp"
    "scaled_push_button.cpp"
    "scaled_spin_box.cpp"
    "scaled_double_spin_box.cpp"
    "scaled_tab_widget.cpp"
    "scaled_table_view.cpp"
    "scaled_tree_view.cpp"
    "shared_isa_item_delegate.cpp"
    "shared_isa_branch_label_navigation_widget.cpp"
    "shared_isa_item_model.cpp"
    "shared_isa_proxy_model.cpp"
    "shared_isa_tree_view.cpp"
    "shared_isa_widget.cpp"
    "tab_bar.cpp"
    "tab_widget.cpp"
    "tree_view.cpp"
    "text_search_widget.cpp"
    "timeline_view.cpp"
)

# Pick up the source files that are relevant to the platform
add_library(${PROJECT_NAME} STATIC ${CPP_SRC} ${CPP_INC} ${UI_SRC})

# Include QtCommon root directory
include_directories(AFTER ../)

# Set binary suffix
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
