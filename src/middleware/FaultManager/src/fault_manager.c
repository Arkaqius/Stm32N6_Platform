/**
 * @file fault_manager.c
 * @brief Implementation of the periodic fault manager
 *
 * Evaluates all registered faults once per tick while applying manager-level
 * inhibition and shadowing rules.
 */

/* Includes -----------------------------------------------------------------*/
#include "flt_man_api.h"
#include "flt_man_types.h"
#include "fault_api.h"
#include "symptom_api.h"

/* Defines ------------------------------------------------------------------*/

/* Local Types and Typedefs -------------------------------------------------*/

/* Global Variables ---------------------------------------------------------*/

/* Private Function Prototypes ----------------------------------------------*/
/**
 * @brief Dispatch a fault transition to the application response manager.
 *
 * @param[in] fault     Fault whose state changed.
 * @param[in] newState  True when the fault became active; false otherwise.
 */
void FaultResponseManager_Dispatch(const Flt_T *fault, bool newState);

/**
 * @brief Check if a fault is shadowed by any other active fault.
 *
 * @param[in] f Fault whose shadowing configuration is inspected.
 *
 * @return True when at least one configured shadow fault is active.
 */
static bool FltMan_IsShadowed(const Flt_T *f);

/**
 * @brief Determine whether a fault should be evaluated this tick.
 *
 * @param[in] fault Fault instance to inspect.
 *
 * @return True when the fault is valid, enabled, and not shadowed.
 */
static bool FltMan_ShouldEvaluate(const Flt_T *fault);

/* Public Functions Implementation ------------------------------------------*/
/**
 * @brief Evaluate all faults managed by the given manager.
 *
 * @param[in] mgr Manager instance containing the fault list.
 */
void FltMan_Tick(const FltMan_T *mgr)
{
    if (!mgr || !mgr->cfg || !mgr->cfg->faults)
    {
        return;
    }

    for (uint8_t i = 0U; i < mgr->cfg->fault_count; ++i)
    {
        const Flt_T *f = &mgr->cfg->faults[i];
        if (FltMan_ShouldEvaluate(f) && Fault_Tick(f))
        {
            FaultResponseManager_Dispatch(f, Fault_IsActive(f));
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
bool FltMan_IsAnyFaultActive(const FltMan_T *mgr)
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
 * @param[in] mgr Manager whose runtime fault states are cleared.
 */
void FltMan_ForceAllClear(const FltMan_T *mgr)
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
 * @param[in]     mgr      Manager instance containing the symptom.
 * @param[in]     symptom  Identifier of the symptom to update.
 * @param[in]     isActive New symptom activity level.
 */
void FltMan_SetSymptom(const FltMan_T *mgr, SymptomsEnum_T symptom, bool isActive)
{
    if (!mgr || !mgr->cfg || !mgr->cfg->symptoms || symptom >= mgr->cfg->symptom_count)
    {
        return;
    }

    Symptom_SetLevel(mgr->cfg->symptoms[symptom].state, isActive, 0U);
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
    if (!f || !f->cfg || !f->cfg->shadow_faults)
    {
        return false;
    }

    for (uint8_t i = 0U; i < f->cfg->shadow_count; ++i)
    {
        if (Fault_IsActive(f->cfg->shadow_faults[i]))
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Determine whether a fault should be evaluated this tick.
 *
 * @param[in] fault Fault instance to inspect.
 *
 * @return True when the fault is valid, enabled, and not shadowed.
 */
static bool FltMan_ShouldEvaluate(const Flt_T *fault)
{
    return fault && fault->cfg && fault->rt && !fault->rt->inhibit &&
           !FltMan_IsShadowed(fault);
}
