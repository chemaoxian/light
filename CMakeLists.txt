cmake_minimum_required(VERSION 2.8)

project(light)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "set buid type to debug")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/build/cmake)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib) 

if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()
if(NOT DEFINED CMAKE_RELEASE_POSTFIX)
    set(CMAKE_RELEASE_POSTFIX "")
endif()

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS  ON)         # only find static libs
set(Boost_USE_STATIC_RUNTIME  OFF)

find_package(Boost REQUIRED COMPONENTS system thread)
find_package(libevent REQUIRED)
find_package(log4cplus REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${LIBEVENT_INCLUDE_DIRS})
include_directories(${LOG4CPLUS_INCLUDE_DIRS})

add_subdirectory(src/light)
add_subdirectory(sample/echoserver)
add_subdirectory(sample/echoclient)



