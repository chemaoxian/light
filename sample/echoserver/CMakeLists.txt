

add_executable(echoserver main.cpp)

if (WIN32)
    target_link_libraries(echoserver Ws2_32)
#    target_link_libraries(echoclient libboost_system libboost_thread)
    target_link_libraries(echoserver libevent)
    target_link_libraries(echoserver light)
    target_link_libraries(echoserver log4cplusD)
elseif(UNIX)
    target_link_libraries(echoserver rt pthread)
    target_link_libraries(echoserver boost_system boost_thread)
    target_link_libraries(echoserver light)
endif()
