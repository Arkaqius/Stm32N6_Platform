cmake_minimum_required(VERSION 3.22)

# Set a variable for the component (library) name
set(COMPONENT_NAME "os")

# Collect sources
file(GLOB COMPONENT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/CMSIS_RTOS_V2/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/portable/*.c"
        # Add more if needed
)

# Create a static library for "os"
add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})

# Public headers for this library
target_include_directories(${COMPONENT_NAME}
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/portable"
    "${CMAKE_CURRENT_SOURCE_DIR}/CMSIS_RTOS_V2"
)

# Link the global STM32 interface + common
target_link_libraries(${COMPONENT_NAME}
    PRIVATE
    cfg_layer
)

# Define an INTERFACE library for the os target
add_library(os_interface INTERFACE)

# Public headers for the INTERFACE library
target_include_directories(os_interface
    INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(os_interface
    INTERFACE
    cfg_layer
)