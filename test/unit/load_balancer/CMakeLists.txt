set(STATIC_LIB "${CMAKE_PROJECT_NAME}-static")
set(TEST_OBJ "${CMAKE_PROJECT_NAME}-test")
set(TEST_RUNNABLE "${CMAKE_PROJECT_NAME}-test-runnable")
set(TEST_STATIC_LIB "${CMAKE_PROJECT_NAME}-test-static")

include(Testing)
add_library(${TEST_OBJ} OBJECT
        fake_configuration.cc
        fake_configuration.h
        fake_server.cc
        fake_server.h
        fake_client.cc
        fake_client.h
)
target_include_directories(${TEST_OBJ} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_link_libraries(${TEST_OBJ} ${STATIC_LIB})
target_link_libraries(${TEST_OBJ} gtest gmock)

add_executable(${TEST_RUNNABLE}
        load_balancer_test.cc
)
target_link_libraries(${TEST_RUNNABLE} PRIVATE ${TEST_OBJ})

# clang-format
include(Format)
Format(${TEST_RUNNABLE} .)

AddTests(${TEST_RUNNABLE})

add_library(${TEST_STATIC_LIB} STATIC)
target_link_libraries(${TEST_STATIC_LIB} PUBLIC ${TEST_OBJ})
