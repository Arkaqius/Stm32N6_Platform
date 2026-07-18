/**
 * @file fault_manager.c
 * @brief Implementation of the periodic fault manager
 *
 * Evaluates all registered faults once per tick and dispatches state changes
 * to the optional Fault Response Manager.
 */

/* Includes -----------------------------------------------------------------*/
#include "flt_man_api.h"
#include "flt_man_types.h"
#include "fault_api.h"
#include "symptom_api.h"
#include "stddef.h"

/* Defines ------------------------------------------------------------------*/

/* Local Types and Typedefs -------------------------------------------------*/

/* Global Variables ---------------------------------------------------------*/

/* Private Function Prototypes ----------------------------------------------*/
/**
 * @brief Dispatch a fault state transition to the application response manager.
 *
 * @param[in,out] fault     Fault whose state changed.
 * @param[in]     new_state True when the fault became active; false otherwise.
 */
/* Forward declaration to decouple from the application component */
void FaultResponseManager_Dispatch(Flt_T *fault, bool new_state);

/**
 * @brief Check if a fault is shadowed by any other active fault.
 *
 * @param[in] f Fault whose shadowing configuration is inspected.
 *
 * @return True when at least one configured shadow fault is active.
 */
static bool FltMan_IsShadowed(const Flt_T *f);

/* Public Functions Implementation ------------------------------------------*/
/**
 * @brief Initialize a fault manager instance.
 *
 * The manager starts with the supplied symptom collection and no registered
 * faults.
 *
 * @param[out] mgr      Manager instance to initialize.
 * @param[in]  symptoms Symptom collection managed by the instance.
 */
void FltMan_Init(FltMan_T *mgr, Symptom_T *symptoms)
{
    if (!mgr || !mgr->runtime)
    {
        return;
    }

    mgr->runtime->symptoms = symptoms;
    mgr->runtime->counter = 0U;
}

/**
 * @brief Evaluate all faults managed by the given manager.
 *
 * @param[in,out] mgr Manager instance containing the fault list.
 */
void FltMan_Tick(FltMan_T *mgr)
{
    if (!mgr || !mgr->cfg)
    {
        return;
    }

    for (uint8_t i = 0; i < mgr->cfg->fault_count; ++i)
    {
        Flt_T *f = &mgr->cfg->faults[i];
        if (!f->rt || f->rt->inhibit)
        {
            continue;
        }

        if (FltMan_IsShadowed(f))
        {
            continue;
        }

        FaultState_T prev_state = f->rt->state;
        Fault_Tick(f);
        if (prev_state != f->rt->state)
        {
            bool new_state = (f->rt->state == FAULT_STATE_ACTIVE);
            if (new_state && f->cfg->capture_freeze_frame)
            {
                f->cfg->capture_freeze_frame(f);
            }
            if (f->cfg->on_transition)
            {
                f->cfg->on_transition(f, new_state);
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
bool FltMan_IsAnyFaultActive(FltMan_T *mgr)
{
    if (!mgr || !mgr->cfg)
    {
        return false;
    }

    for (uint8_t i = 0; i < mgr->cfg->fault_count; ++i)
    {
        if (Fault_IsActive(&mgr->cfg->faults[i]))
        {
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
void FltMan_ForceAllClear(FltMan_T *mgr)
{
    if (!mgr || !mgr->cfg)
    {
        return;
    }

    for (uint8_t i = 0; i < mgr->cfg->fault_count; ++i)
    {
        Fault_ForceState(&mgr->cfg->faults[i], false);
    }
}

/**
 * @brief Set the activity level of a symptom managed by the instance.
 *
 * Invalid manager pointers and symptom identifiers are ignored.
 *
 * @param[in,out] mgr      Manager instance containing the symptom.
 * @param[in]     symptom  Identifier of the symptom to update.
 * @param[in]     isActive New symptom activity level.
 */
void FltMan_SetSymptom(FltMan_T *mgr, SymptomsEnum_T symptom, bool isActive)
{
    if (!mgr || !mgr->cfg || !mgr->runtime || !mgr->runtime->symptoms ||
        symptom >= mgr->cfg->symptom_count)
    {
        return;
    }

    Symptom_SetLevel(mgr->runtime->symptoms[symptom].state, isActive, 0U);
}

/* Private Functions Implementation -----------------------------------------*/
/**
 * @brief Check if a fault is shadowed by any other active fault.
 *
 * @param[in] f Fault whose shadowing configuration is inspected.
 *
 * @return True when at least one configured shadow fault is active.
 */
static bool FltMan_IsShadowed(const Flt_T *f)
{
    if (!f || !f->cfg)
    {
        return false;
    }

    for (uint8_t i = 0; i < f->cfg->shadow_count; ++i)
    {
        if (Fault_IsActive(f->cfg->shadow_faults[i]))
        {
            return true;
        }
    }
    return false;
}
