cmake_minimum_required(VERSION 3.22)
# ------------------------------------------------------------------------------
# Project Configuration
# ------------------------------------------------------------------------------
project(EdgeAI LANGUAGES C CXX ASM)
# Set CMake to be less verbose
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_RULE_MESSAGES OFF)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# Main Executable
# ------------------------------------------------------------------------------
# If you want a top-level executable (e.g., for the final firmware image):
add_executable(${PROJECT_NAME}
    ./app/DevM/src/DevM.c
)

# Link the interface library so the main executable inherits MCU flags & definitions
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        os
        DevM
)

# ------------------------------------------------------------------------------
# Subdirectories for each layer or component
# ------------------------------------------------------------------------------
# Adjust to your folder structure
add_subdirectory(app ${CMAKE_BINARY_DIR}/app)
add_subdirectory(bsw ${CMAKE_BINARY_DIR}/bsw)
add_subdirectory(cfg ${CMAKE_BINARY_DIR}/cfg)
add_subdirectory(libs ${CMAKE_BINARY_DIR}/libs)
add_subdirectory(middleware ${CMAKE_BINARY_DIR}/middleware)
add_subdirectory(os ${CMAKE_BINARY_DIR}/os)

target_link_libraries(${PROJECT_NAME}
     PRIVATE
       app_layer
       #middleware
       #bsw
       os
       HAL_Drv
       infra

)