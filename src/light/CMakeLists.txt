
aux_source_directory(${CMAKE_SOURCE_DIR}/src/light LIGHT_SRCS)

file(GLOB_RECURSE project_headers *.h *.hpp)
file(GLOB_RECURSE project_cpps *.cpp *.cxx *.cc)

add_library(light STATIC ${project_headers} ${project_cpps})
target_link_libraries(light event event_core event_extra log4cplus)
