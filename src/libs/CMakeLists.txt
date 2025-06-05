cmake_minimum_required(VERSION 3.22)

# Set a variable for the component (library) name
set(COMPONENT_NAME "libs_layer")

# Subdirs 
# The directory is called "HAL_Drv" in this repository. Use the correct
# casing so configuration succeeds on case-sensitive file systems.
add_subdirectory(HAL_Drv ${CMAKE_BINARY_DIR}/HAL_Drv)
add_subdirectory(CMSIS ${CMAKE_BINARY_DIR}/CMSIS)


add_library(${COMPONENT_NAME} INTERFACE)
target_link_libraries(${COMPONENT_NAME}
    INTERFACE
        test_swc
)

