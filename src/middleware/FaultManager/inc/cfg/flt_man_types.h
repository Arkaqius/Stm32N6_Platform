/**
 * @file flt_man_types.h
 * @brief Configuration type declarations for the periodic Fault Manager
 *
 * Defines the configuration and runtime objects used to manage a collection
 * of Fault_T instances.
 */

#ifndef FLT_MAN_TYPES_H
#define FLT_MAN_TYPES_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "flt_man_api.h"
#include "flt_types.h"
#include "symptom_types.h"

/* Macros and Defines -------------------------------------------------------*/

/* Typedefs -----------------------------------------------------------------*/
/**
 * @brief Immutable configuration for a Fault Manager instance.
 */
typedef struct
{
    Flt_T *const faults;          /**< Array of managed faults. */
    Symptom_T *const symptoms;    /**< Array of managed symptoms. */
    const uint8_t fault_count;    /**< Number of entries in the fault array. */
    const uint32_t symptom_count; /**< Number of managed symptoms. */
} FltMan_Cfg_T;

/**
 * @brief Mutable runtime state for a Fault Manager instance.
 */
typedef struct
{
    uint8_t counter; /**< Manager runtime counter. */
} FltMan_Runtime_T;

/**
 * @brief Fault Manager object combining configuration and runtime state.
 */
struct FltMan_Tag
{
    const FltMan_Cfg_T *cfg;   /**< Read-only manager configuration. */
    FltMan_Runtime_T *runtime; /**< Mutable manager runtime state. */
};

/* Exported Variables -------------------------------------------------------*/

/* Exported Interfaces ------------------------------------------------------*/

#endif /* FLT_MAN_TYPES_H */
