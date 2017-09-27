
#aux_source_directory(${CMAKE_SOURCE_DIR}/sample/echoserver ECHO_SRCS)

#file(GLOB_RECURSE project_headers *.h *.hpp)
#file(GLOB_RECURSE project_cpps *.cpp *.cxx *.cc)

add_executable(echoclient main.cpp)
target_link_libraries(echoclient libevent libevent_core light log4cplusD.lib)