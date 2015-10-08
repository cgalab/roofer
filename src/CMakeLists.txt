set(SRC_LIST  main.cpp Skeleton.cpp Data.cpp GUI.cpp)

CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
  message(SEND_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

add_executable(bis ${SRC_LIST})
target_link_libraries(bis ${CGAL_LIBRARIES}
                          ${CGAL_3RD_PARTY_LIBRARIES}
                          ${MAP_VIEWER_LIBRARIES}
                     )

