cmake_minimum_required(VERSION 2.8)

project(light)

set(CMAKE_BUILD_TYPE "DEBUG")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/build/cmake)

message("${CMAKE_CXX_FLAGS_DEBUG}")

set(Boost_USE_MULTITHREADED ON)

find_package(Boost REQUIRED)
find_package(libevent REQUIRED)

set(Boost_USE_STATIC_LIBS  ON)         # only find static libs
set(Boost_USE_MULTITHREADED  ON)
set(Boost_USE_STATIC_RUNTIME  OFF)

include_directories(./src)
include_directories(${LIBEVENT_INCLUDE_DIRS})
include_directories(${LOG4CPLUS_INCLUDE_DIR})
link_directories(${LIBEVENT_LIB_DIR})
link_directories(${LOG4CPLUS_LIB_DIR})

add_subdirectory(src/light)
add_subdirectory(sample/echoserver)
add_subdirectory(sample/echoclient)



