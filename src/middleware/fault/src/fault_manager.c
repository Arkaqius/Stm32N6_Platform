/**
 * @file fault_manager.c
 * @brief Implementation of the periodic fault manager
 *
 * Evaluates all registered faults once per tick and dispatches state changes
 * to the optional Fault Response Manager.
 */

/* Includes -----------------------------------------------------------------*/
#include "fault_manager.h"

/* Forward declaration to decouple from the application component */
void FaultResponseManager_Dispatch(Fault_T *fault, bool new_state);

/* Private Functions --------------------------------------------------------*/
/**
 * @brief Check if a fault is shadowed by any other active fault.
 */
static bool is_shadowed(const Fault_T *f)
{
    for (uint8_t i = 0; i < f->shadow_count; ++i) {
        if (Fault_IsActive(f->shadow_faults[i])) {
            return true;
        }
    }
    return false;
}

/* Public Functions Implementation ------------------------------------------*/
/**
 * @brief Evaluate all faults managed by the given manager.
 *
 * @param[in,out] mgr Manager instance containing the fault list.
 */
void FaultManager_Tick(FaultManager_T *mgr)
{
    if (!mgr) {
        return;
    }

    for (uint8_t i = 0; i < mgr->fault_count; ++i) {
        Fault_T *f = mgr->faults[i];
        if (!f || f->inhibit) {
            continue;
        }

        if (is_shadowed(f)) {
            continue;
        }

        FaultState_T prev_state = f->state;
        Fault_Tick(f);
        if (prev_state != f->state) {
            bool new_state = (f->state == FAULT_STATE_ACTIVE);
            if (new_state && f->capture_freeze_frame) {
                f->capture_freeze_frame(f);
            }
            if (f->on_transition) {
                f->on_transition(f, new_state);
            }
            FaultResponseManager_Dispatch(f, new_state);
        }
    }
}

/**
 * @brief Determine if any fault is active.
 *
 * @param[in] mgr Manager instance to query.
 *
 * @return true when at least one fault reports active.
 */
bool FaultManager_IsAnyFaultActive(FaultManager_T *mgr)
{
    if (!mgr) {
        return false;
    }

    for (uint8_t i = 0; i < mgr->fault_count; ++i) {
        if (Fault_IsActive(mgr->faults[i])) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Force all managed faults inactive.
 *
 * @param[in,out] mgr Manager instance.
 */
void FaultManager_ForceAllClear(FaultManager_T *mgr)
{
    if (!mgr) {
        return;
    }

    for (uint8_t i = 0; i < mgr->fault_count; ++i) {
        Fault_ForceState(mgr->faults[i], false);
    }
}
