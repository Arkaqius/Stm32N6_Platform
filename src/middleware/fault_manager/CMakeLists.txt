cmake_minimum_required(VERSION 3.22)

set(COMPONENT_NAME "fault_mgmt")
set(API_TARGET_NAME "flt_man_api")
set(CFG_TARGET_NAME "flt_man_cfg")
set(INTERNAL_TARGET_NAME "flt_man_internal")

set(PUBLIC_HEADERS
    inc/public/fault_manager_api.h
)

set(CFG_HEADERS
    inc/cfg/fault_manager_types.h
    inc/cfg/fault_manager_fault_types.h
    inc/cfg/fault_manager_symptom_types.h
)

set(INTERNAL_HEADERS
    inc/internal/fault_manager_fault_api.h
    inc/internal/fault_manager_symptom_api.h
)

set(INTERNAL_SOURCES
    src/fault_manager.c
    src/internal/fault_manager_fault.c
    src/internal/fault_manager_symptom.c
)

# Public API facade shared by consumers without introducing a binary-library
# dependency.
add_library(${API_TARGET_NAME} INTERFACE)

target_include_directories(${API_TARGET_NAME}
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/inc/public"
)

# Configuration facade: configuration code can instantiate the concrete types,
# but does not receive the internal fault and symptom subcomponent APIs.
add_library(${CFG_TARGET_NAME} INTERFACE)

target_sources(${CFG_TARGET_NAME}
    INTERFACE
        ${CFG_HEADERS}
)

target_include_directories(${CFG_TARGET_NAME}
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/inc/cfg"
)

target_link_libraries(${CFG_TARGET_NAME}
    INTERFACE
        ${API_TARGET_NAME}
)

# Compile the implementation behind a private header boundary. Consumers of
# fault_mgmt do not inherit any of these include directories.
add_library(${INTERNAL_TARGET_NAME} OBJECT)

target_sources(${INTERNAL_TARGET_NAME}
    PRIVATE
        ${INTERNAL_SOURCES}
        ${INTERNAL_HEADERS}
        ${CFG_HEADERS}
        ${PUBLIC_HEADERS}
)

target_include_directories(${INTERNAL_TARGET_NAME}
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/inc/internal"
)

target_link_libraries(${INTERNAL_TARGET_NAME}
    PRIVATE
        os
        ${CFG_TARGET_NAME}
)

# Public facade: only the manager API include directory is propagated.
add_library(${COMPONENT_NAME} STATIC
    $<TARGET_OBJECTS:${INTERNAL_TARGET_NAME}>
)

target_sources(${COMPONENT_NAME}
    PRIVATE
        ${PUBLIC_HEADERS}
)

target_include_directories(${COMPONENT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/inc/public"
)

target_link_libraries(${COMPONENT_NAME}
    PUBLIC
        os
        ${API_TARGET_NAME}
    PRIVATE
        fault_response
)
