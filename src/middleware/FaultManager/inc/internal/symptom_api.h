/**
 * @file symptom_api.h
 * @brief Internal operations for Fault Manager symptoms
 *
 * Provides initialization and state access helpers for the symptom primitive.
 */

#ifndef SYMPTOM_API_H
#define SYMPTOM_API_H

/* Includes -----------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "symptom_types.h"

/* Macros and Defines -------------------------------------------------------*/
/**
 * @brief Compile-time initializer for a symptom runtime state object.
 */
#define SYMPTOM_RUNTIME_INIT \
    {.isActive = false, .last_update_tick = 0u}

/* Typedefs -----------------------------------------------------------------*/

/* Exported Variables -------------------------------------------------------*/

/* Exported Interfaces ------------------------------------------------------*/
/**
 * @brief Set a symptom's current activity level.
 *
 * @param[in,out] s      Symptom runtime state to update.
 * @param[in]     active New symptom activity level.
 * @param[in]     tick   Tick at which the update occurred.
 */
void Symptom_SetLevel(SymptomState_T *s, bool active, uint32_t tick);

/**
 * @brief Query the current activity level of a symptom.
 *
 * @param[in] s Symptom runtime state to query.
 *
 * @return True when the state exists and the symptom is active.
 */
static inline bool Symptom_IsActive(const SymptomState_T *s)
{
    return s ? s->isActive : false;
}

#endif /* SYMPTOM_API_H */
