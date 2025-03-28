cmake_minimum_required(VERSION 3.22)

# Set a variable for the component (library) name
set(COMPONENT_NAME "HAL_Drv")

# Collect sources
file(GLOB COMPONENT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    # Add more if needed
)

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})

# Public headers for this library
target_include_directories(${COMPONENT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/inc"
)

# Link the global STM32 interface + test_swc
target_link_libraries(${COMPONENT_NAME}
    PRIVATE
        cfg_layer
)
