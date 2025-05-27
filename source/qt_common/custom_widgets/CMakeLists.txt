cmake_minimum_required (VERSION 3.24)

# Build Qt .ui MOC files for the library.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Add all header and source files within the directory to the library.
file (GLOB CPP_INC
    "arrow_icon_combo_box.h"
    "banner_widget.h"
    "check_box_widget.h"
    "colored_legend_graphics_view.h"
    "colored_legend_scene.h"
    "completion_bar_widget.h"
    "donut_pie_widget.h"
    "double_slider_widget.h"
    "driver_overrides_model.h"
    "driver_overrides_notification_banner.h"
    "driver_overrides_notification_config_widget.h"
    "driver_overrides_tree_widget.h"
    "elided_line_label.h"
    "expanding_scroll_area.h"
    "file_loading_widget.h"
    "graphics_scene.h"
    "graphics_view.h"
    "icon_button.h"
    "include_directories_view.h"
    "list_widget.h"
    "message_overlay.h"
    "message_overlay_container.h"
    "navigation_bar.h"
    "navigation_list_widget.h"
    "navigation_list_view.h"
    "navigation_list_model.h"
    "ordered_list_dialog.h"
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
    "scaled_link_label.h"
    "scaled_menu_bar.h"
    "scaled_push_button.h"
    "scaled_spin_box.h"
    "scaled_double_spin_box.h"
    "scaled_tab_widget.h"
    "scaled_table_view.h"
    "scaled_tree_view.h"
    "tab_bar.h"
    "tree_view.h"
    "tab_widget.h"
    "text_search_widget.h"
    "timeline_view.h"
    "tooltip_widget.h"
)

# Add any .ui  files
file (GLOB UI_SRC
    "banner_widget.ui"
    "driver_overrides_notification_config_widget.ui"
    "driver_overrides_tree_widget.ui"
    "message_overlay.ui"
    "ordered_list_view_dialog.ui"
)

# Add all source files found within this directory.
file (GLOB CPP_SRC
    "arrow_icon_combo_box.cpp"
    "banner_widget.cpp"
    "check_box_widget.cpp"
    "colored_legend_graphics_view.cpp"
    "colored_legend_scene.cpp"
    "completion_bar_widget.cpp"
    "donut_pie_widget.cpp"
    "double_slider_widget.cpp"
    "driver_overrides_model.cpp"
    "driver_overrides_notification_banner.cpp"
    "driver_overrides_notification_config_widget.cpp"
    "driver_overrides_tree_widget.cpp"
    "elided_line_label.cpp"
    "expanding_scroll_area.cpp"
    "file_loading_widget.cpp"
    "graphics_scene.cpp"
    "graphics_view.cpp"
    "icon_button.cpp"
    "include_directories_view.cpp"
    "list_widget.cpp"
    "message_overlay.cpp"
    "message_overlay_container.cpp"
    "navigation_bar.cpp"
    "navigation_list_widget.cpp"
    "navigation_list_view.cpp"
    "navigation_list_model.cpp"
    "ordered_list_dialog.cpp"
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
    "scaled_link_label.cpp"
    "scaled_menu_bar.cpp"
    "scaled_push_button.cpp"
    "scaled_spin_box.cpp"
    "scaled_double_spin_box.cpp"
    "scaled_tab_widget.cpp"
    "scaled_table_view.cpp"
    "scaled_tree_view.cpp"
    "tab_bar.cpp"
    "tab_widget.cpp"
    "tree_view.cpp"
    "text_search_widget.cpp"
    "timeline_view.cpp"
    "tooltip_widget.cpp"
)

# Pick up the source files that are relevant to the platform
add_library(QtCustomWidgets STATIC ${CPP_SRC} ${CPP_INC} ${UI_SRC})

# Include QtCommon root directory
include_directories(AFTER ../..)
target_include_directories(QtCustomWidgets PUBLIC ${PROJECT_SOURCE_DIR}/source PRIVATE . ../utils)

# Set binary suffix
target_link_libraries(QtCustomWidgets Qt::Widgets)

devtools_target_options(QtCustomWidgets)
