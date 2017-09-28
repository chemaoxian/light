cmake_minimum_required(VERSION 2.8)

project(light)

set(Boost_USE_MULTITHREADED ON)

find_package(Boost REQUIRED)

set(Boost_USE_STATIC_LIBS  ON)         # only find static libs
set(Boost_USE_MULTITHREADED  ON)
set(Boost_USE_STATIC_RUNTIME  OFF)

if(NOT Boost_FOUND)
    message("Boost not found")
endif()

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/thirdparty/libevent-2.0.22-stable/include
    ${CMAKE_SOURCE_DIR}/thirdparty/log4cplus-1.1.2/include
    ${CMAKE_SOURCE_DIR}/src
    )
  
link_directories(
    ${Boost_LIBRARY_DIRS}
)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "current platform : ${CMAKE_SYSTEM_NAME}")
    include_directories(
        ${CMAKE_SOURCE_DIR}/thirdparty/libevent-2.0.22-stable/Win32-Code
        )
    link_directories(
        ${CMAKE_SOURCE_DIR}/thirdparty/libevent-2.0.22-stable/lib/win
        ${CMAKE_SOURCE_DIR}/thirdparty/log4cplus-1.1.2/msvc10/Win32/bin.Debug
        )
elseif (CMAKE_SYSTEM_NAME MATCHES "linux")
    message(STATUS "current platform : ${CMAKE_SYSTEM_NAME}")
    link_directories(
        ${CMAKE_SOURCE_DIR}/lib
        ${CMAKE_SOURCE_DIR}/thirdparty/log4cplus-1.1.2/src/.libs
    )
else()
     message(STATUS "other platform : ${CMAKE_SYSTEM_NAME}")
endif()

add_subdirectory(src/light)
add_subdirectory(sample/echoserver)
add_subdirectory(sample/echoclient)



