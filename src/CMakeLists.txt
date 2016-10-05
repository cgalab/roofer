set(SRC_LIST  main.cpp Skeleton.cpp Data.cpp SweepLine.cpp Facets.cpp)

if( WITH_GUI )
   list(APPEND SRC_LIST GUI.cpp)
   add_definitions(-DQTGUI)
endif( WITH_GUI )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if( WITH_DEBUG )
   set(CMAKE_BUILD_TYPE Debug FORCE)
   set_directory_properties(PROPERTIES COMPILE_DEFINITIONS_DEBUG DEBUG=1)
   add_definitions ("-g")
   message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
endif( WITH_DEBUG)

add_executable(roofer ${SRC_LIST})
target_link_libraries(roofer ${CGAL_LIBRARIES}
                          ${CGAL_3RD_PARTY_LIBRARIES}
                          ${MAP_VIEWER_LIBRARIES}
                     )

