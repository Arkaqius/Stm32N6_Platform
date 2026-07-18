/**
 * @file fault.c
 * @brief Fault evaluation implementation
 *
 * Contains the core debounce algorithm used by the Fault Manager.
 */

/* Includes -----------------------------------------------------------------*/
#include "fault_api.h"
#include "symptom_api.h"

/* Defines ------------------------------------------------------------------*/

/* Local Types and Typedefs -------------------------------------------------*/

/* Global Variables ---------------------------------------------------------*/

/* Private Function Prototypes ----------------------------------------------*/

/* Public Functions Implementation ------------------------------------------*/
/**
 * @brief Evaluate a fault instance for the current tick.
 *
 * @param[in,out] f Fault instance to process.
 */
void Fault_Tick(Flt_T *f)
{
    if (!f)
    {
        return;
    }

    if (!f->cfg || !f->rt)
    {
        return;
    }

    bool input_active = false;
    for (uint8_t i = 0; i < f->cfg->input_count; ++i)
    {
        const Symptom_T *src = &f->rt->inputs[i];
        if (src && Symptom_IsActive(src->state))
        {
            input_active = true;
            break;
        }
    }

    if (input_active)
    {
        if (f->cfg->window == 0 || f->rt->counter < f->cfg->window)
        {
            if (f->rt->counter < UINT8_MAX)
            {
                f->rt->counter++;
            }
        }
    }
    else if (f->rt->counter > 0)
    {
        f->rt->counter--;
    }

    if (f->rt->counter >= f->cfg->threshold)
    {
        f->rt->state = FAULT_STATE_ACTIVE;
    }
    else if (f->rt->counter == 0)
    {
        f->rt->state = FAULT_STATE_INACTIVE;
    }
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
 * @param[in,out] f Fault instance to modify.
 * @param[in] enabled When true the fault is skipped during evaluation.
 */
void Fault_SetInhibit(Flt_T *f, bool enabled)
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
 * @param[in,out] f Fault instance to modify.
 * @param[in] state Desired active state.
 */
void Fault_ForceState(Flt_T *f, bool state)
{
    if (!f || !f->cfg || !f->rt)
    {
        return;
    }

    f->rt->counter = state ? f->cfg->threshold : 0;
    f->rt->state = state ? FAULT_STATE_ACTIVE : FAULT_STATE_INACTIVE;
}

/* Private Functions Implementation -----------------------------------------*/
