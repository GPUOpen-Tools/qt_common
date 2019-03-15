cmake_minimum_required (VERSION 3.5.1)
project (QtScaling)

set (QTSCALING_VERSION_MAJOR 1)
set (QTSCALING_VERSION_MINOR 0)

# Build Qt .ui MOC files for the library.
set(CMAKE_AUTOMOC ON)

# Add all header and source files within the directory to the library.
file (GLOB CPP_INC
	"*.h"
)

# Add all source files found within this directory.
file (GLOB CPP_SRC
	"*.cpp"
)

# Pick up the source files that are relevant to the platform
add_library(${PROJECT_NAME} STATIC ${CPP_SRC})

# Find Qt libraries to resolve dependencies.
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Set binary suffix
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
