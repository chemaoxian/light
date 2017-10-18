

add_executable(echoclient main.cpp)

if (WIN32)
    target_link_libraries(echoclient Ws2_32)
    target_link_libraries(echoclient libevent libevent_core)
    target_link_libraries(echoclient light)
    target_link_libraries(echoclient log4cplusD)
elseif(UNIX)
    target_link_libraries(echoclient rt pthread)
    target_link_libraries(echoclient boost_system boost_thread)
    target_link_libraries(echoclient light)
endif()
