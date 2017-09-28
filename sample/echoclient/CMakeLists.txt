
#aux_source_directory(${CMAKE_SOURCE_DIR}/sample/echoserver ECHO_SRCS)

#file(GLOB_RECURSE project_headers *.h *.hpp)
#file(GLOB_RECURSE project_cpps *.cpp *.cxx *.cc)

add_executable(echoclient main.cpp)

if (WIN32)
    target_link_libraries(echoclient Ws2_32)
#    target_link_libraries(echoclient libboost_system libboost_thread)
    target_link_libraries(echoclient libevent libevent_core)
    target_link_libraries(echoclient light)
    target_link_libraries(echoclient log4cplusD)
elseif(UNIX)
    target_link_libraries(echoclient rt pthread)
    target_link_libraries(echoclient boost_system boost_thread)
    target_link_libraries(echoclient event event_core event_pthreads)
    target_link_libraries(echoclient light)
    target_link_libraries(echoclient log4cplus.a)
endif()