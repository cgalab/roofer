# Created by the script cgal_create_cmake_script
# This is the CMake script for compiling a CGAL application.

project( Bisector-Graph-MAX )

cmake_minimum_required(VERSION 2.6.2)
if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}" VERSION_GREATER 2.8.3)
  cmake_policy(VERSION 2.8.4)
else()
  cmake_policy(VERSION 2.6)
endif()

include(CheckCXXCompilerFlag)

find_package(CGAL QUIET COMPONENTS Core)

# To use valgrind, we must disable rounding math ckeck.
add_definitions(-DCGAL_DISABLE_ROUNDING_MATH_CHECK)

set(SRC_LIST  main.cpp) # Skeleton.cpp Data.cpp GUI.cpp)


if ( CGAL_FOUND )

  include( ${CGAL_USE_FILE} )

  # include( CGAL_CreateSingleSourceCGALProgram )
  include_directories(BEFORE ../../include)

  # If you want to visualize a linear cell complex, there are 2 viewers 
  # based on qt and vtk. Just uncomment the corresponding line
  include("CMakeLCCViewerQt.inc")
  #  include("CMakeLCCViewerVtk.inc")

  CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
  CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
  if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
  endif()

  add_executable(bismax ${SRC_LIST})
  target_link_libraries(bismax ${CGAL_LIBRARIES}
                                   ${CGAL_3RD_PARTY_LIBRARIES}
                                   ${MAP_VIEWER_LIBRARIES}
                                   )
else()
  message(STATUS "This program requires the CGAL library, "
                 "and will not be compiled.")
endif()

# More verbose output by make
set(CMAKE_VERBOSE_MAKEFILE "OFF")
