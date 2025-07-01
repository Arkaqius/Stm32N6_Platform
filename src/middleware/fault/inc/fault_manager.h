/**
 * @file fault_manager.h
 * @brief Periodic manager for evaluating faults
 *
 * Provides an interface to tick and query a collection of Fault_T instances.
 */

#ifndef FAULT_MANAGER_H
#define FAULT_MANAGER_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "fault.h"

/* Typedefs -----------------------------------------------------------------*/
/**
 * @brief Container holding an array of faults managed together.
 */
typedef struct
{
    Fault_T  *const *faults;    /**< Array of managed fault pointers */
    uint8_t   fault_count;      /**< Number of entries in the array */
} FaultManager_T;

/* Exported Functions -------------------------------------------------------*/
/**
 * @brief Process all registered faults for one scheduler tick.
 *
 * @param[in,out] mgr Fault manager instance.
 */
void FaultManager_Tick(FaultManager_T *mgr);

/**
 * @brief Check if any managed fault is currently active.
 *
 * @param[in] mgr Fault manager instance.
 *
 * @return true if at least one fault is active.
 */
bool FaultManager_IsAnyFaultActive(FaultManager_T *mgr);

/**
 * @brief Force all managed faults to the inactive state.
 *
 * Useful for unit tests or when clearing faults after service actions.
 *
 * @param[in,out] mgr Fault manager instance.
 */
void FaultManager_ForceAllClear(FaultManager_T *mgr);

#endif /* FAULT_MANAGER_H */
