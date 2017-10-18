
aux_source_directory(${CMAKE_SOURCE_DIR}/src/light LIGHT_SRCS)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(light STATIC ${LIGHT_SRCS})

if(WIN32)
    target_link_libraries(light libevent_core)
    target_link_libraries(light libevent_extra)
else()
    target_link_libraries(light libevent_core.a)
    target_link_libraries(light libevent_extra.a)
    target_link_libraries(light libevent_pthreads.a)
endif()

target_link_libraries(light liblog4cplus.a)
