cmake_minimum_required(VERSION 3.22)

# Set a variable for the component (library) name
set(COMPONENT_NAME "app_layer")

# Subdirs 
add_subdirectory(test_swc)
add_subdirectory(DevM)

add_library(${COMPONENT_NAME} INTERFACE)
target_link_libraries(${COMPONENT_NAME}
    INTERFACE
        test_swc
        os
        HAL_Drv
        cfg_layer
        cmsis
)        