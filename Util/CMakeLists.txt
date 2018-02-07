cmake_minimum_required (VERSION 3.5.1)
project (QtUtil)

set (QTUTIL_VERSION_MAJOR 1)
set (QTUTIL_VERSION_MINOR 0)

# Build Qt .ui MOC files for the library.
set(CMAKE_AUTOMOC ON)

# specify compile options
IF(UNIX)
  add_compile_options(-std=c++11 -D_LINUX -Wall -Wextra)
ENDIF(UNIX)

# Add all header and source files within the directory to the library.
file (GLOB CPP_INC
	"*.h"
)

# Add all source files found within this directory.
file (GLOB CPP_SRC
	"*.cpp"
)

# Pick up the source files that are relevant to the platform
add_library(QtUtil STATIC ${CPP_SRC})

# Find Qt libraries to resolve dependencies.
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Set binary suffix
set_target_properties(QtUtil PROPERTIES DEBUG_POSTFIX -d)
target_link_libraries(QtUtil Qt5::Widgets)
