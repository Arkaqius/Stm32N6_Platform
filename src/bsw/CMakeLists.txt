cmake_minimum_required(VERSION 3.22)

# Set a variable for the component (library) name
set(COMPONENT_NAME "bsw_layer")

# Subdirs 
add_subdirectory(infra)
add_subdirectory(uart_dma)

add_library(${COMPONENT_NAME} INTERFACE)
