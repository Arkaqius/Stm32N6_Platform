/**
 * @file flt_types.h
 * @brief Configuration type declarations for debounced faults
 *
 * Defines the immutable configuration and mutable runtime state used by the
 * Fault Manager to evaluate and report faults.
 */

#ifndef FLT_TYPES_H
#define FLT_TYPES_H

/* Includes -----------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "flt_man_api.h"
#include "symptom_types.h"

/* Macros and Defines -------------------------------------------------------*/

/* Typedefs -----------------------------------------------------------------*/
/**
 * @brief Function called when a fault changes state.
 *
 * @param[in,out] fault     Fault whose state changed.
 * @param[in]     new_state True when the fault became active; false otherwise.
 */
typedef void (*FaultTransitionHook)(struct Flt_T *fault, bool new_state);

/**
 * @brief Function called to capture diagnostic data on fault activation.
 *
 * @param[in,out] fault Fault for which diagnostic data is captured.
 */
typedef void (*FaultFreezeFrameHook)(struct Flt_T *fault);

/**
 * @brief Possible fault states after evaluation.
 */
typedef enum
{
    FAULT_STATE_INACTIVE = 0, /**< Fault not present */
    FAULT_STATE_ACTIVE        /**< Fault currently active */
} FaultState_T;

/**
 * @brief Configuration and runtime data for a fault.
 */
typedef struct FaultCfg_T
{
    Symptom_T *inputs;         /**< Array of symptom input pointers */
    const uint8_t input_count; /**< Number of input pointers */
    const uint8_t threshold;   /**< Count required for activation */
    const uint8_t window;      /**< Maximum counter value */

    const struct Flt_T *const *shadow_faults; /**< Faults that shadow this one */
    const uint8_t shadow_count;               /**< Number of shadowing faults */

    FaultTransitionHook on_transition;         /**< Optional transition callback */
    FaultFreezeFrameHook capture_freeze_frame; /**< Optional freeze frame hook */
} FaultCfg_T;

/**
 * @brief Mutable state used while evaluating a fault.
 */
typedef struct FaultRuntime_T
{

    FaultState_T state; /**< Current evaluated state */
    uint8_t counter;    /**< Internal debounce counter */
    bool inhibit;       /**< Skip evaluation when true */
} FaultRuntime_T;

/**
 * @brief Fault object combining immutable configuration and runtime state.
 */
struct Flt_T
{
    const FaultCfg_T *cfg; /**< ROM config */
    FaultRuntime_T *rt;    /**< RAM state */
};

/* Exported Variables -------------------------------------------------------*/

/* Exported Interfaces ------------------------------------------------------*/

#endif /* FLT_TYPES_H */
