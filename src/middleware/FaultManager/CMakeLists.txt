cmake_minimum_required(VERSION 3.22)

set(COMPONENT_NAME "fault_mgmt")

set(COMPONENT_SOURCES
    src/fault.c
    src/fault_manager.c
    src/symptom.c
)

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})

target_include_directories(${COMPONENT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/inc"
)

target_include_directories(${COMPONENT_NAME}
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/inc/internal"
)

target_link_libraries(${COMPONENT_NAME}
    PUBLIC
        os
)

