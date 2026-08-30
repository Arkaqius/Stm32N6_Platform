/**
 * @file fault_manager_symptom_types.h
 * @brief Configuration type declarations for Fault Manager symptoms
 *
 * Declares the configuration, runtime state, and aggregate object used to
 * represent a symptom in the Fault Manager.
 */

#ifndef SYMPTOM_TYPES_H
#define SYMPTOM_TYPES_H

/* Includes -----------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "fault_manager_api.h"

/* Macros and Defines -------------------------------------------------------*/

/* Typedefs -----------------------------------------------------------------*/
/**
 * @brief Immutable configuration of a symptom.
 */
typedef struct SymptomCfg_Tag
{
    const uint32_t id; /**< Application-defined symptom identifier. */
} SymptomCfg_T;

/**
 * @brief Mutable runtime state of a symptom.
 */
typedef struct SymptomState_Tag
{
    volatile bool isActive;             /**< Current symptom activity level. */
    volatile uint32_t last_update_tick; /**< Tick of the most recent update. */
} SymptomState_T;

/**
 * @brief Symptom object combining immutable configuration and runtime state.
 */
struct Symptom_Tag
{
    const SymptomCfg_T *cfg; /**< Read-only symptom configuration. */
    SymptomState_T *state;   /**< Mutable symptom runtime state. */
};

/* Exported Variables -------------------------------------------------------*/

/* Exported Interfaces ------------------------------------------------------*/

#endif /* SYMPTOM_TYPES_H */
