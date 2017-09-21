
#aux_source_directory(${CMAKE_SOURCE_DIR}/sample/echoserver ECHO_SRCS)

#file(GLOB_RECURSE project_headers *.h *.hpp)
#file(GLOB_RECURSE project_cpps *.cpp *.cxx *.cc)

add_executable(echoserver main.cpp)
target_link_libraries(echoserver libevent libevent_core light log4cplusD.lib)