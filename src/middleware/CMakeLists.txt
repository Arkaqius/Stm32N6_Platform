cmake_minimum_required(VERSION 3.22)

# Set a variable for the component (library) name
set(COMPONENT_NAME "middleware_layer")

# Subdirs 
add_subdirectory(logger)

add_library(${COMPONENT_NAME} INTERFACE)
target_link_libraries(${COMPONENT_NAME}
    INTERFACE
        os
        HAL_Drv
        cfg_layer
        cmsis
)        