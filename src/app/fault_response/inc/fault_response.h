/**
 * @file fault_response.h
 * @brief Application fault response manager API
 *
 * Allows the application layer to register hooks that react to fault state
 * changes signalled by the middleware Fault Manager.
 */

#ifndef FAULT_RESPONSE_H
#define FAULT_RESPONSE_H

/* Includes -----------------------------------------------------------------*/
#include "fault_manager_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* Typedefs -----------------------------------------------------------------*/
    /** Hook function invoked when a fault transitions state. */
    typedef void (*FaultResponseHook)(const Flt_T *fault, bool active);

#ifndef FAULT_RESPONSE_MANAGER_MAX_ENTRIES
#define FAULT_RESPONSE_MANAGER_MAX_ENTRIES 16 /**< Size of the response table */
#endif

    /* Exported Functions -------------------------------------------------------*/
    /**
     * @brief Register a response hook for the given fault.
     *
     * @param[in] fault Fault instance to associate with the hook.
     * @param[in] hook  Callback function invoked on transitions.
     */
    void FaultResponseManager_Register(const Flt_T *fault, FaultResponseHook hook);

    /**
     * @brief Dispatch a fault state change to the registered hook.
     *
     * Typically called by the Fault Manager once a fault transitions.
     *
     * @param[in] fault     Fault instance that changed state.
     * @param[in] new_state true if the fault became active.
     */
    void FaultResponseManager_Dispatch(const Flt_T *fault, bool new_state);

#ifdef __cplusplus
}
#endif

#endif /* FAULT_RESPONSE_H */
