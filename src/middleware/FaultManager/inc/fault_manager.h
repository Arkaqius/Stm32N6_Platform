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
#include "symptom.h"
/* Typedefs -----------------------------------------------------------------*/
/**
 * @brief Container holding an array of faults managed together.
 */
typedef struct
{
    Symptom_T *symptoms;         /**< Symptom array for monitoring */
    const uint8_t symptom_count; /**< Number of symptoms */
    Fault_T *const *faults;      /**< Array of managed fault pointers */
    uint8_t fault_count;         /**< Number of entries in the array */
} FaultManager_T;

typedef uint32_t SymptomsEnum_T;
#ifndef SYM_CFG_ATTR
#define SYM_CFG_ATTR
#endif
#ifndef SYM_STATE_ATTR
#define SYM_STATE_ATTR
#endif

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

void FaultManger_Init(FaultManager_T *mgr, Symptom_T *symptoms);

void FaultManger_SetSymptom(FaultManager_T *mgr, SymptomsEnum_T symptom, bool isActive);

#endif /* FAULT_MANAGER_H */
