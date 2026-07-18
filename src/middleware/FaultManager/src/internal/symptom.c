/**
 * @file symptom.c
 * @brief Symptom primitive implementation
 *
 * Provides basic set/clear/query operations for the passive Symptom_T object.
 */

/* Includes -----------------------------------------------------------------*/
#include "symptom_api.h"

/* Defines ------------------------------------------------------------------*/

/* Local Types and Typedefs -------------------------------------------------*/

/* Global Variables ---------------------------------------------------------*/

/* Private Function Prototypes ----------------------------------------------*/

/* Public Functions Implementation ------------------------------------------*/
/**
 * @brief Set the current symptom level and its update timestamp.
 *
 * A null state pointer is ignored.
 *
 * @param[in,out] s      Symptom runtime state to update.
 * @param[in]     active New symptom activity level.
 * @param[in]     tick   Tick at which the symptom was updated.
 */
void Symptom_SetLevel(SymptomState_T *s, bool active, uint32_t tick)
{
    if (!s)
        return;
    s->isActive = active;
    s->last_update_tick = tick;
}

/* Private Functions Implementation -----------------------------------------*/
