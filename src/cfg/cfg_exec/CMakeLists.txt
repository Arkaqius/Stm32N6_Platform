cmake_minimum_required(VERSION 3.22)

set(COMPONENT_NAME "cfg_exec")

add_library(${COMPONENT_NAME} STATIC
    src/cfg_exec.c
)

target_include_directories(${COMPONENT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/inc"
)

target_link_libraries(${COMPONENT_NAME}
    Exec_cfg
    flt_man_api
)
