cmake_minimum_required(VERSION 3.22)

set(COMPONENT_NAME "SysM")

file(GLOB COMPONENT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})

target_include_directories(${COMPONENT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/inc"
)

target_link_libraries(${COMPONENT_NAME}
    PRIVATE
        os
        cfg_layer
        test_swc
        logger
)