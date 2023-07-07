cmake_minimum_required(VERSION 3.5)
project(chx1_demo)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 14)
include_directories(Include)

find_package(PCL REQUIRED)
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

IF(CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-DWIN64)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_definitions(-DLINUX)
ENDIF()

 find all cpp files under *Include* directory
file(GLOB SRC_NAMES Include/*.cpp)

message("*********: ",${SRC_NAMES})
add_executable(demo demo/main.cpp)

target_link_libraries(demo -pthread)


add_executable(demo_viewer demo_PCL/main_PCL.cpp)

target_link_libraries(demo_viewer -pthread ${PCL_LIBRARIES})