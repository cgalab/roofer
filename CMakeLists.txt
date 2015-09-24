project( Bisector-MAX )
#LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "/usr/lib/CGAL")

cmake_minimum_required(VERSION 2.6.2)
if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}" VERSION_GREATER 2.8.3)
  cmake_policy(VERSION 2.8.4)
else()
  cmake_policy(VERSION 2.6)
endif()

include_directories(BEFORE ../../include)

include(CheckCXXCompilerFlag)

find_package(CGAL COMPONENTS Qt4 Core REQUIRED)
include(${CGAL_USE_FILE})
include(${QT_USE_FILE})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_AUTOMOC ON) # QT Macro
#set(CMAKE_AUTOUIC ON) # QT uic code gen.

# If you want to visualize a linear cell complex, there are 2 viewers 
# based on qt and vtk. Just uncomment the corresponding line
include("CMakeLCCViewerQt.inc")
# include("CMakeLCCViewerVtk.inc")

# More verbose output by make
set(CMAKE_VERBOSE_MAKEFILE "OFF")

subdirs(src)
