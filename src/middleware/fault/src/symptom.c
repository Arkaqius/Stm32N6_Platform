/**
 * @file symptom.c
 * @brief Symptom primitive implementation
 *
 * Provides basic set/clear/query operations for the passive Symptom_T object.
 */

/* Includes -----------------------------------------------------------------*/
#include "symptom.h"

/* Public Functions Implementation ------------------------------------------*/
/**
 * @brief Set the symptom active.
 *
 * @param[in,out] s Symptom instance to modify.
 */
void Symptom_Set(Symptom_T *s)
{
    if (s) {
        s->active = true;
    }
}

/**
 * @brief Clear the symptom.
 *
 * @param[in,out] s Symptom instance to modify.
 */
void Symptom_Clear(Symptom_T *s)
{
    if (s) {
        s->active = false;
    }
}

/**
 * @brief Query whether a symptom is active.
 *
 * @param[in] s Symptom instance to query.
 *
 * @return true if active, false otherwise.
 */
bool Symptom_IsActive(const Symptom_T *s)
{
    return s ? s->active : false;
}
