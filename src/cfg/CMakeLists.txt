cmake_minimum_required(VERSION 3.22)

# Set a variable for the component (library) name
set(COMPONENT_NAME "cfg_layer")

# Create an INTERFACE library for "cfg_layer"
add_library(${COMPONENT_NAME} INTERFACE)

# Public headers for this library
target_include_directories(${COMPONENT_NAME}
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/inc"
)

# Link the global STM32 interface + common
target_link_libraries(${COMPONENT_NAME}
    INTERFACE
        cmsis
        logger
)