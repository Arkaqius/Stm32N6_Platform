/**
 * @file fault.h
 * @brief Debounced fault evaluation primitive
 *
 * Contains the configuration structure and helper functions used by the
 * Fault Manager to evaluate system faults.  Each Fault_T can source multiple
 * symptoms, apply debounce logic and optional shadowing or inhibition.
 */

#ifndef FAULT_H
#define FAULT_H

/* Includes -----------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

/* Typedefs -----------------------------------------------------------------*/
struct Fault_T;

/** Function called on fault state transition. */
typedef void (*FaultTransitionHook)(struct Fault_T *fault, bool new_state);
/** Function capturing a freeze frame on activation. */
typedef void (*FaultFreezeFrameHook)(struct Fault_T *fault);

/** Possible fault states after evaluation. */
typedef enum
{
    FAULT_STATE_INACTIVE = 0, /**< Fault not present */
    FAULT_STATE_ACTIVE        /**< Fault currently active */
} FaultState_T;

/**
 * @brief Configuration and runtime data for a fault.
 */
typedef struct Fault_T
{
    FaultState_T state;                     /**< Current evaluated state */
    const bool *const *inputs;              /**< Array of symptom input pointers */
    uint8_t      input_count;               /**< Number of input pointers */
    uint8_t      counter;                   /**< Internal debounce counter */
    uint8_t      threshold;                 /**< Count required for activation */
    uint8_t      window;                    /**< Maximum counter value */
    bool         inhibit;                   /**< Skip evaluation when true */
    const struct Fault_T *const *shadow_faults; /**< Faults that shadow this one */
    uint8_t      shadow_count;              /**< Number of shadowing faults */
    FaultTransitionHook   on_transition;    /**< Optional transition callback */
    FaultFreezeFrameHook  capture_freeze_frame; /**< Optional freeze frame hook */
} Fault_T;

/* Exported Functions -------------------------------------------------------*/
/**
 * @brief Evaluate a single fault and update its state.
 *
 * Typically called by the Fault Manager each tick.
 *
 * @param[in,out] f Fault instance to process.
 */
void Fault_Tick(Fault_T *f);

/**
 * @brief Query whether the fault is currently active.
 *
 * @param[in] f Fault instance to query.
 *
 * @return true if active, false otherwise.
 */
bool Fault_IsActive(const Fault_T *f);

/**
 * @brief Enable or disable fault evaluation.
 *
 * @param[in,out] f      Fault instance to modify.
 * @param[in]     enabled True to inhibit evaluation, false to enable.
 */
void Fault_SetInhibit(Fault_T *f, bool enabled);

/**
 * @brief Force the fault to a specific state.
 *
 * Primarily intended for unit tests or debug features.
 *
 * @param[in,out] f     Fault instance to modify.
 * @param[in]     state Desired active state.
 */
void Fault_ForceState(Fault_T *f, bool state);

#endif /* FAULT_H */
