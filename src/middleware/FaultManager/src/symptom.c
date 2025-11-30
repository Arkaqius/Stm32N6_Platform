/**
 * @file symptom.c
 * @brief Symptom primitive implementation
 *
 * Provides basic set/clear/query operations for the passive Symptom_T object.
 */

/* Includes -----------------------------------------------------------------*/
#include "symptom.h"

/* Public Functions Implementation ------------------------------------------*/
void Symptom_SetLevel(SymptomState_T *s, bool active, uint32_t tick)
{
    if (!s)
        return;
    s->isActive = active;
    s->last_update_tick = tick;
}