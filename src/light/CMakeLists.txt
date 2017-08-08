
aux_source_directory(${CMAKE_SOURCE_DIR}/src/light LIGHT_SRCS)

file(GLOB_RECURSE project_headers *.h *.hpp)
file(GLOB_RECURSE project_cpps *.cpp *.cxx *.cc)

add_library(light ${project_headers} ${project_cpps})