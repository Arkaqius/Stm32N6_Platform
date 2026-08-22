/**
 * @file fault_response.c
 * @brief Application fault response manager
 *
 * Provides a simple lookup table of callbacks that are invoked when faults
 * transition state.  This allows application code to react to fault events
 * without adding dependencies to the middleware layer.
 */

/* Includes -----------------------------------------------------------------*/
#include "fault_response.h"

/* Local Types and Typedefs -------------------------------------------------*/
/** Entry linking a fault to its response hook. */
typedef struct
{
    const Flt_T *fault;     /**< Fault to monitor */
    FaultResponseHook hook; /**< Hook invoked on transition */
} FaultResponseEntry;

/* Global Variables ---------------------------------------------------------*/
static FaultResponseEntry response_table[FAULT_RESPONSE_MANAGER_MAX_ENTRIES];
static uint8_t response_count = 0;

/* Public Functions Implementation ------------------------------------------*/
/**
 * @brief Register a response hook for a fault.
 *
 * @param[in] fault Fault instance to monitor.
 * @param[in] hook  Callback invoked on state changes.
 */
void FaultResponseManager_Register(const Flt_T *fault, FaultResponseHook hook)
{
    if (response_count >= FAULT_RESPONSE_MANAGER_MAX_ENTRIES)
    {
        return;
    }

    response_table[response_count].fault = fault;
    response_table[response_count].hook = hook;
    response_count++;
}

/**
 * @brief Dispatch a fault transition to its registered hook.
 *
 * @param[in] fault     Fault instance that changed.
 * @param[in] new_state true if the fault became active.
 */
void FaultResponseManager_Dispatch(const Flt_T *fault, bool new_state)
{
    for (uint8_t i = 0; i < response_count; ++i)
    {
        if (response_table[i].fault == fault && response_table[i].hook)
        {
            response_table[i].hook(fault, new_state);
            break;
        }
    }
}
