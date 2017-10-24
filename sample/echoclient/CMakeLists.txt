

add_executable(echoclient main.cpp)

if (WIN32)
    target_link_libraries(echoclient Ws2_32)
elseif(UNIX)
    target_link_libraries(echoclient rt)
endif()

target_link_libraries(echoclient light)
target_link_libraries(echoclient ${CMAKE_THREAD_LIBS_INIT})

