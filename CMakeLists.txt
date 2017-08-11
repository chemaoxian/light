cmake_minimum_required(VERSION 2.8)

project(light)

set(Boost_USE_MULTITHREADED ON)

find_package(Boost REQUIRED COMPONENTS
    filesystem
    system
    thread
    )

if(NOT Boost_FOUND)
    message("Boost not found")
endif()

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/thirdparty/libevent-2.0.22-stable/include
    ${CMAKE_SOURCE_DIR}/thirdparty/log4cplus-1.2.1-rc2/include
    ${CMAKE_SOURCE_DIR}/src
    )

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "current platform : ${CMAKE_SYSTEM_NAME}")
    include_directories(
        ${CMAKE_SOURCE_DIR}/thirdparty/libevent-2.0.22-stable/Win32-Code
        )
    link_directories(
        ${CMAKE_SOURCE_DIR}/thirdparty/libevent-2.0.22-stable/lib/win
        ${CMAKE_SOURCE_DIR}/log4cplus-1.2.1-rc2/msvc10/Win32/bin.Debug
        )
elseif (CMAKE_SYSTEM_NAME MATCHES "linux")
    message(STATUS "current platform : ${CMAKE_SYSTEM_NAME}")
    link_directories(
        ${CMAKE_SOURCE_DIR}/thirdparty/libevent-2.0.22-stable/lib/lin
    )
else()
     message(STATUS "other platform : ${CMAKE_SYSTEM_NAME}")
endif()

add_subdirectory(src/light)



