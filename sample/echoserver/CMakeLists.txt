
#aux_source_directory(${CMAKE_SOURCE_DIR}/sample/echoserver ECHO_SRCS)

#file(GLOB_RECURSE project_headers *.h *.hpp)
#file(GLOB_RECURSE project_cpps *.cpp *.cxx *.cc)

add_executable(echoserver main.cpp)
target_link_libraries(echoserver rt pthread)
target_link_libraries(echoserver boost_system boost_thread) 
target_link_libraries(echoserver event event_core event_pthreads)
target_link_libraries(echoserver light)
target_link_libraries(echoserver log4cplus)
