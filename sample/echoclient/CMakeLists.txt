

add_executable(echoclient main.cpp)

if (WIN32)
target_link_libraries(echoclient Ws2_32)
elseif(UNIX)
target_link_libraries(echoclient rt pthread)
endif()

target_link_libraries(echoclient light)

