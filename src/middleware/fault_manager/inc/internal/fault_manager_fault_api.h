/**
 * @file fault_manager_fault_api.h
 * @brief Debounced fault evaluation primitive
 *
 * Contains the configuration structure and helper functions used by the
 * Fault Manager to evaluate system faults.  Each Fault_T can source multiple
 * symptoms, apply debounce logic and optional shadowing or inhibition.
 */

#ifndef FAULT_API_H
#define FAULT_API_H

/* Includes -----------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "fault_manager_fault_types.h"

/* Macros and Defines -------------------------------------------------------*/

/* Typedefs -----------------------------------------------------------------*/

/* Exported Variables -------------------------------------------------------*/

/* Exported Interfaces ------------------------------------------------------*/
/**
 * @brief Evaluate a single fault and update its state.
 *
 * Typically called by the Fault Manager each tick. This operation performs
 * input sampling, debounce, state evaluation, freeze-frame capture, and
 * transition notification.
 *
 * @param[in] f Fault whose runtime state is processed.
 *
 * @return True when the fault state changed during this tick.
 */
bool Fault_Tick(const Flt_T *f);

/**
 * @brief Query whether the fault is currently active.
 *
 * @param[in] f Fault instance to query.
 *
 * @return true if active, false otherwise.
 */
bool Fault_IsActive(const Flt_T *f);

/**
 * @brief Enable or disable fault evaluation.
 *
 * @param[in]     f       Fault whose runtime state is modified.
 * @param[in]     enabled True to inhibit evaluation, false to enable.
 */
void Fault_SetInhibit(const Flt_T *f, bool enabled);

/**
 * @brief Force the fault to a specific state.
 *
 * Primarily intended for unit tests or debug features.
 *
 * @param[in]     f     Fault whose runtime state is modified.
 * @param[in]     state Desired active state.
 */
void Fault_ForceState(const Flt_T *f, bool state);

#endif /* FAULT_API_H */
