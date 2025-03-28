cmake_minimum_required(VERSION 3.22)

# Set a variable for the component (library) name
set(COMPONENT_NAME "libs_layer")

# Subdirs 
add_subdirectory(Hal_Drv ${CMAKE_BINARY_DIR}/HAL_Drv)
add_subdirectory(CMSIS ${CMAKE_BINARY_DIR}/CMSIS)


add_library(${COMPONENT_NAME} INTERFACE)
target_link_libraries(${COMPONENT_NAME}
    INTERFACE
        test_swc
)

