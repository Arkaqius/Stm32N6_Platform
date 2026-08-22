cmake_minimum_required(VERSION 3.22)

set(COMPONENT_NAME "fault_response")

set(COMPONENT_SOURCES
    src/fault_response.c
)

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})

target_include_directories(${COMPONENT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/inc"
)

target_link_libraries(${COMPONENT_NAME}
    PUBLIC
        flt_man_api
)

