set(STATIC_LIB "socket-wrapper-static")

add_library(${STATIC_LIB} STATIC
        include/udp_socket.h
        include/socket.h
        include/end_point.h
        include/udp.h
        include/protocol.h
        include/invalid_socket_exception.h
        include/shut_down_socket_exception.h
)
set_target_properties(${STATIC_LIB} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${STATIC_LIB} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

#warnings
target_compile_options(${STATIC_LIB} PRIVATE "-Werror" "-Wall" "-Wextra" "-Wpedantic")

# clang-format
include(Format)
Format(${STATIC_LIB} .)
