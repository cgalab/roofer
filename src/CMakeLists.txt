set(SRC_LIST  main.cpp Skeleton.cpp Data.cpp SweepLine.cpp Arrangement.cpp)

if( WITH_GUI )
   list(APPEND SRC_LIST GUI.cpp)
   add_definitions(-DQTGUI)
endif( WITH_GUI )

CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
 
if(COMPILER_SUPPORTS_CXX11)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -std=c++11 -stdlib=libc++")
elseif(COMPILER_SUPPORTS_CXX0X)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
   #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -std=c++11 -stdlib=libc++")
else()
  message(SEND_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

if( WITH_DEBUG )
   set(CMAKE_BUILD_TYPE Debug FORCE)
   set_directory_properties(PROPERTIES COMPILE_DEFINITIONS_DEBUG DEBUG=1)
   add_definitions ("-g")
   message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
endif( WITH_DEBUG)

add_executable(bis ${SRC_LIST})
target_link_libraries(bis ${CGAL_LIBRARIES}
                          ${CGAL_3RD_PARTY_LIBRARIES}
                          ${MAP_VIEWER_LIBRARIES}
                     )

