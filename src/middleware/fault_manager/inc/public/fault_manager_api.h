/**
 * @file fault_manager_api.h
 * @brief Public interface for the periodic Fault Manager
 *
 * Provides opaque handles and operations for evaluating and querying the
 * Fault Manager without exposing its concrete implementation types.
 */

#ifndef FLT_MAN_API_H
#define FLT_MAN_API_H

/* Includes -----------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

/* Macros and Defines -------------------------------------------------------*/

/* Typedefs -----------------------------------------------------------------*/
/** Opaque Fault Manager instance. */
typedef struct FltMan_Tag FltMan_T;

/** Opaque fault instance. */
typedef struct Flt_T Flt_T;

/** Opaque symptom instance. */
typedef struct Symptom_Tag Symptom_T;

/** Application-defined symptom identifier. */
typedef uint32_t SymptomsEnum_T;

/* Exported Variables -------------------------------------------------------*/

/* Exported Interfaces ------------------------------------------------------*/
/**
 * @brief Evaluate every registered fault for one manager tick.
 *
 * @param[in] mgr Manager instance to evaluate.
 */
void FltMan_Tick(const FltMan_T *mgr);

/**
 * @brief Determine whether any managed fault is active.
 *
 * @param[in] mgr Manager instance to query.
 *
 * @return True when at least one managed fault is active.
 */
bool FltMan_IsAnyFaultActive(const FltMan_T *mgr);

/**
 * @brief Force all managed faults to the inactive state.
 *
 * @param[in] mgr Manager whose runtime fault states are modified.
 */
void FltMan_ForceAllClear(const FltMan_T *mgr);

/**
 * @brief Set the activity level of a managed symptom.
 *
 * @param[in]     mgr      Manager instance containing the symptom.
 * @param[in]     symptom  Identifier of the symptom to update.
 * @param[in]     isActive New symptom activity level.
 */
void FltMan_SetSymptom(const FltMan_T *mgr, SymptomsEnum_T symptom, bool isActive);

#endif /* FLT_MAN_API_H */
