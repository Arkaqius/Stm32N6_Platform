/**
 * @file fault.c
 * @brief Fault evaluation implementation
 *
 * Contains the core debounce algorithm used by the Fault Manager.
 */

/* Includes -----------------------------------------------------------------*/
#include "fault.h"

/* Public Functions Implementation ------------------------------------------*/
/**
 * @brief Evaluate a fault instance for the current tick.
 *
 * @param[in,out] f Fault instance to process.
 */
void Fault_Tick(Fault_T *f)
{
    if (!f) {
        return;
    }

    bool input_active = false;
    for (uint8_t i = 0; i < f->input_count; ++i) {
        const bool *src = f->inputs[i];
        if (src && *src) {
            input_active = true;
            break;
        }
    }

    if (input_active) {
        if (f->window == 0 || f->counter < f->window) {
            if (f->counter < UINT8_MAX) {
                f->counter++;
            }
        }
    } else if (f->counter > 0) {
        f->counter--;
    }

    if (f->counter >= f->threshold) {
        f->state = FAULT_STATE_ACTIVE;
    } else if (f->counter == 0) {
        f->state = FAULT_STATE_INACTIVE;
    }
}

/**
 * @brief Query whether the fault state is active.
 *
 * @param[in] f Fault instance to query.
 *
 * @return true if active, false otherwise.
 */
bool Fault_IsActive(const Fault_T *f)
{
    return f ? (f->state == FAULT_STATE_ACTIVE) : false;
}

/**
 * @brief Enable or disable evaluation of the fault.
 *
 * @param[in,out] f Fault instance to modify.
 * @param[in] enabled When true the fault is skipped during evaluation.
 */
void Fault_SetInhibit(Fault_T *f, bool enabled)
{
    if (f) {
        f->inhibit = enabled;
    }
}

/**
 * @brief Force a fault to a given state.
 *
 * Primarily used by test or debug code to override normal evaluation.
 *
 * @param[in,out] f Fault instance to modify.
 * @param[in] state Desired active state.
 */
void Fault_ForceState(Fault_T *f, bool state)
{
    if (!f) {
        return;
    }

    f->counter = state ? f->threshold : 0;
    f->state = state ? FAULT_STATE_ACTIVE : FAULT_STATE_INACTIVE;
}
