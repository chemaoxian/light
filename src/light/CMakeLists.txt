
aux_source_directory(${CMAKE_SOURCE_DIR}/src/light LIGHT_SRCS)

add_library(light STATIC ${LIGHT_SRCS})

target_link_libraries(light ${LIBEVENT_LIBRARIES})
target_link_libraries(light ${LOG4CPLUS_LIBRARIES})
target_link_libraries(light ${Boost_LIBRARIES})
