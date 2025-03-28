cmake_minimum_required(VERSION 3.22)

# Set a variable for the component (library) name
set(COMPONENT_NAME "cmsis")

file(GLOB COMPONENT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/RTOS2/Source/*.c"
)

# Create an INTERFACE library for "cmsis"
add_library(${COMPONENT_NAME} STATIC  ${COMPONENT_SOURCES})

# Public headers for this library
target_include_directories(${COMPONENT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/Device/ST/STM32N6xx/Include"
        "${CMAKE_CURRENT_SOURCE_DIR}/Include"
        "${CMAKE_CURRENT_SOURCE_DIR}/RTOS2/Include"
)

# Link the global STM32 interface + common
target_link_libraries(${COMPONENT_NAME}
    PRIVATE
        HAL_Drv
        cfg_layer
        os
)   