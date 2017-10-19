

add_executable(echoserver main.cpp)

if (WIN32)
    target_link_libraries(echoserver Ws2_32)
elseif(UNIX)
    target_link_libraries(echoserver rt pthread)
endif()

target_link_libraries(echoserver light)
target_link_libraries(echoserver ${Boost_LIBRARIES})
