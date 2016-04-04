project( Bisector )
#LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "/usr/lib/CGAL")

cmake_minimum_required(VERSION 2.8.12)
if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}" VERSION_GREATER 2.8.3)
  cmake_policy(VERSION 2.8.4)
else()
  cmake_policy(VERSION 2.6)
endif()

# Clang config
#set(CMAKE_C_COMPiler "clang-3.5")
#set(cmake_cxx_compiler "clang++-3.5")
#set(cmake_cxx_flags "${cmake_cxx_flags} -std=c++11 -stdlib=libc++")
#set(cmake_exe_linker_flags "${cMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")

option(WITH_GUI "Enable QT GUI (requieres libQGLViewer)!" "ON")


include_directories(BEFORE ../../include)

include(CheckCXXCompilerFlag)

# compiler warnings 
add_definitions ("-Wall")

#find_package(CGAL COMPONENTS Qt4 Core REQUIRED)
find_package(CGAL COMPONENTS Qt5 Core REQUIRED)
include(${CGAL_USE_FILE})
include(${QT_USE_FILE})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_AUTOMOC ON) # QT Macro
#set(CMAKE_AUTOUIC ON) # QT uic code gen.

# If you want to visualize a linear cell complex, there are 2 viewers 
# based on qt and vtk. Just uncomment the corresponding line
if( WITH_GUI )
   include("CMakeLCCViewerQt.inc")
endif( WITH_GUI )

# include("CMakeLCCViewerVtk.inc")


# More verbose output by make
set(CMAKE_VERBOSE_MAKEFILE "OFF")

subdirs(src)
