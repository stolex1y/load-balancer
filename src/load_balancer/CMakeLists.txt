set(OBJ_LIB ${CMAKE_PROJECT_NAME})
set(RUNNABLE "${CMAKE_PROJECT_NAME}-runnable")
set(STATIC_LIB "${CMAKE_PROJECT_NAME}-static")
set(SOCKET_WRAPPER_LIB "socket-wrapper-static")

add_library(${OBJ_LIB} OBJECT
        load_balancer.h
        load_balancer.cc
)
set_target_properties(${OBJ_LIB} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${OBJ_LIB} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_link_libraries(${OBJ_LIB} PUBLIC ${SOCKET_WRAPPER_LIB})

#warnings
target_compile_options(${OBJ_LIB} PRIVATE "-Werror" "-Wall" "-Wextra" "-Wpedantic")

add_executable(${RUNNABLE}
        main.cc
)
target_link_libraries(${RUNNABLE} PRIVATE ${OBJ_LIB})

# clang-format
include(Format)
Format(${RUNNABLE} .)

# copy config.properties
add_custom_command(
        TARGET ${RUNNABLE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/config.properties
        ${CMAKE_CURRENT_BINARY_DIR}/config.properties
)

add_library(${STATIC_LIB} STATIC)
target_link_libraries(${STATIC_LIB} ${OBJ_LIB})
