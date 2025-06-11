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

add_compile_definitions(
    STM32N657xx
    USE_FULL_LL_DRIVER
    HSE_VALUE=48000000U
    HSE_STARTUP_TIMEOUT=100U
    LSE_VALUE=32768U
    LSE_STARTUP_TIMEOUT=5000U
    EXTERNAL_CLOCK_VALUE=12288000U
    MSI_VALUE=16000000U
    HSI_VALUE=64000000U
    LSI_VALUE=32000U
    VDD_VALUE=3300U
    TX_SINGLE_MODE_SECURE=1U
)
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
       middleware_layer
       bsw_layer
       os
       HAL_Drv
       infra

)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYFILE ${CMAKE_SOURCE_DIR}/../docs/Doxyfile)
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../docs
        COMMENT "Generate API documentation"
        VERBATIM
    )
endif()

