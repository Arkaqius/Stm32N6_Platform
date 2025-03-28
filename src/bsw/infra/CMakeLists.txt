cmake_minimum_required(VERSION 3.22)

# Set a variable for the component (library) name
set(COMPONENT_NAME "infra")

# Collect sources
file(GLOB COMPONENT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.s"
)

# Create a static library for "app"
add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})

# Public headers for this library
target_include_directories(${COMPONENT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/inc"
)

target_link_libraries(${COMPONENT_NAME}
    PRIVATE
    cmsis
    cfg_layer
)
