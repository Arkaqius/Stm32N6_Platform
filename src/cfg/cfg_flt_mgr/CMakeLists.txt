cmake_minimum_required(VERSION 3.22)

# Set a variable for the component (library) name
set(COMPONENT_NAME "cfg_flt_mgr")

add_library(${COMPONENT_NAME} STATIC
    src/cfg_flt_mgr.c
)

target_include_directories(${COMPONENT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/inc"
)

target_link_libraries(${COMPONENT_NAME}
    PRIVATE
        cmsis
        logger
        flt_man_cfg
)
