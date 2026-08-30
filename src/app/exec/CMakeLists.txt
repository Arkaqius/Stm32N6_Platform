cmake_minimum_required(VERSION 3.22)

set(COMPONENT_NAME "Exec")
set(API_TARGET_NAME "Exec_api")
set(CFG_TARGET_NAME "Exec_cfg")

set(COMPONENT_SOURCES
    src/exec.c
)

set(API_HEADERS
    inc/public/exec_api.h
)

set(CFG_HEADERS
    inc/cfg/exec_cfg.h
)

add_library(${API_TARGET_NAME} INTERFACE)

target_sources(${API_TARGET_NAME}
    INTERFACE
        ${API_HEADERS}
)

target_include_directories(${API_TARGET_NAME}
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/inc/public"
)

add_library(${CFG_TARGET_NAME} INTERFACE)

target_sources(${CFG_TARGET_NAME}
    INTERFACE
        ${CFG_HEADERS}
)

target_include_directories(${CFG_TARGET_NAME}
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/inc/cfg"
)

target_link_libraries(${CFG_TARGET_NAME}
    INTERFACE
        ${API_TARGET_NAME}
)

add_library(${COMPONENT_NAME} STATIC
    ${COMPONENT_SOURCES}
    ${API_HEADERS}
    ${CFG_HEADERS}
)

target_link_libraries(${COMPONENT_NAME}
    PUBLIC
        ${API_TARGET_NAME} os
    PRIVATE
        ${CFG_TARGET_NAME}
)
