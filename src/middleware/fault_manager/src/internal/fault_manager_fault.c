/**
 * @file fault_manager_fault.c
 * @brief Fault evaluation implementation
 *
 * Contains the core debounce algorithm used by the Fault Manager.
 */

/* Includes -----------------------------------------------------------------*/
#include "fault_manager_fault_api.h"
#include "fault_manager_symptom_api.h"

/* Defines ------------------------------------------------------------------*/

/* Local Types and Typedefs -------------------------------------------------*/

/* Global Variables ---------------------------------------------------------*/

/* Private Function Prototypes ----------------------------------------------*/
/** @brief Validate the configuration and runtime references of a fault. */
static bool Fault_IsConfigured(const Flt_T *fault);

/** @brief Determine whether any configured symptom input is active. */
static bool Fault_HasActiveInput(const Flt_T *fault);

/** @brief Update the debounce counter from the sampled input level. */
static void Fault_UpdateCounter(const Flt_T *fault, bool inputActive);

/** @brief Update the fault state and report whether it changed. */
static bool Fault_UpdateState(const Flt_T *fault);

/** @brief Invoke callbacks associated with a fault-state transition. */
static void Fault_NotifyTransition(const Flt_T *fault);

/* Public Functions Implementation ------------------------------------------*/
/**
 * @brief Evaluate a fault instance for the current tick.
 *
 * @param[in] f Fault whose runtime state is processed.
 *
 * @return True when the fault state changed during this tick.
 */
bool Fault_Tick(const Flt_T *f)
{
    if (!Fault_IsConfigured(f))
    {
        return false;
    }

    const bool inputActive = Fault_HasActiveInput(f);
    Fault_UpdateCounter(f, inputActive);
    const bool stateChanged = Fault_UpdateState(f);

    if (stateChanged)
    {
        Fault_NotifyTransition(f);
    }

    return stateChanged;
}

/**
 * @brief Query whether the fault state is active.
 *
 * @param[in] f Fault instance to query.
 *
 * @return true if active, false otherwise.
 */
bool Fault_IsActive(const Flt_T *f)
{
    return (f && f->rt) ? (f->rt->state == FAULT_STATE_ACTIVE) : false;
}

/**
 * @brief Enable or disable evaluation of the fault.
 *
 * @param[in] f       Fault whose runtime inhibit flag is modified.
 * @param[in] enabled When true the fault is skipped during evaluation.
 */
void Fault_SetInhibit(const Flt_T *f, bool enabled)
{
    if (f && f->rt)
    {
        f->rt->inhibit = enabled;
    }
}

/**
 * @brief Force a fault to a given state.
 *
 * Primarily used by test or debug code to override normal evaluation.
 *
 * @param[in] f     Fault whose runtime state is modified.
 * @param[in] state Desired active state.
 */
void Fault_ForceState(const Flt_T *f, bool state)
{
    if (!f || !f->cfg || !f->rt)
    {
        return;
    }

    f->rt->counter = state ? f->cfg->threshold : 0;
    f->rt->state = state ? FAULT_STATE_ACTIVE : FAULT_STATE_INACTIVE;
}

/* Private Functions Implementation -----------------------------------------*/
/**
 * @brief Validate the configuration and runtime references of a fault.
 *
 * @param[in] fault Fault instance to validate.
 *
 * @return True when the fault can be evaluated safely.
 */
static bool Fault_IsConfigured(const Flt_T *fault)
{
    if (!fault || !fault->cfg || !fault->rt)
    {
        return false;
    }

    if ((fault->cfg->input_count > 0U) && !fault->cfg->inputs)
    {
        return false;
    }

    if ((fault->cfg->threshold == 0U) ||
        ((fault->cfg->window != 0U) && (fault->cfg->threshold > fault->cfg->window)))
    {
        return false;
    }

    return true;
}

/**
 * @brief Determine whether any configured symptom input is active.
 *
 * @param[in] fault Fault instance whose inputs are sampled.
 *
 * @return True when at least one symptom input is active.
 */
static bool Fault_HasActiveInput(const Flt_T *fault)
{
    for (uint8_t i = 0U; i < fault->cfg->input_count; ++i)
    {
        if (Symptom_IsActive(fault->cfg->inputs[i].state))
        {
            return true;
        }
    }

    return false;
}

/**
 * @brief Update the debounce counter from the sampled input level.
 *
 * A zero window selects the full uint8_t counter range.
 *
 * @param[in] fault       Fault whose counter is updated.
 * @param[in] inputActive Sampled aggregate input level.
 */
static void Fault_UpdateCounter(const Flt_T *fault, bool inputActive)
{
    const uint8_t counterLimit = (fault->cfg->window == 0U) ? UINT8_MAX : fault->cfg->window;

    if (inputActive && (fault->rt->counter < counterLimit))
    {
        ++fault->rt->counter;
    }
    else if (!inputActive && (fault->rt->counter > 0U))
    {
        --fault->rt->counter;
    }
}

/**
 * @brief Update the fault state from its debounce counter.
 *
 * @param[in] fault Fault whose state is updated.
 *
 * @return True when the evaluated state changed.
 */
static bool Fault_UpdateState(const Flt_T *fault)
{
    const FaultState_T previousState = fault->rt->state;

    if (fault->rt->counter >= fault->cfg->threshold)
    {
        fault->rt->state = FAULT_STATE_ACTIVE;
    }
    else if (fault->rt->counter == 0U)
    {
        fault->rt->state = FAULT_STATE_INACTIVE;
    }

    return previousState != fault->rt->state;
}

/**
 * @brief Invoke callbacks associated with a fault-state transition.
 *
 * Freeze-frame capture is performed only when the fault becomes active. The
 * transition callback is invoked for both activation and deactivation.
 *
 * @param[in] fault Fault whose state changed.
 */
static void Fault_NotifyTransition(const Flt_T *fault)
{
    const bool isActive = (fault->rt->state == FAULT_STATE_ACTIVE);

    if (isActive && fault->cfg->capture_freeze_frame)
    {
        fault->cfg->capture_freeze_frame(fault);
    }

    if (fault->cfg->on_transition)
    {
        fault->cfg->on_transition(fault, isActive);
    }
}
