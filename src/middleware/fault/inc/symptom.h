/**
 * @file symptom.h
 * @brief Symptom primitive used by the Fault Manager
 *
 * Passive boolean object that represents a raw monitoring result. The
 * application or diagnostic monitors update the symptom state externally.
 */

#ifndef SYMPTOM_H
#define SYMPTOM_H

/* Includes -----------------------------------------------------------------*/
#include <stdbool.h>

/* Typedefs -----------------------------------------------------------------*/
/**
 * @brief Boolean symptom flag updated by monitoring logic.
 */
typedef struct
{
    bool active; /**< Current state of the symptom */
} Symptom_T;

/* Exported Functions -------------------------------------------------------*/
/**
 * @brief Set the symptom active.
 *
 * @param[in,out] s Symptom instance to update.
 */
void Symptom_Set(Symptom_T *s);

/**
 * @brief Clear the symptom.
 *
 * @param[in,out] s Symptom instance to update.
 */
void Symptom_Clear(Symptom_T *s);

/**
 * @brief Query whether the symptom is currently active.
 *
 * @param[in] s Symptom instance to query.
 *
 * @return true if active, false otherwise.
 */
bool Symptom_IsActive(const Symptom_T *s);

#endif /* SYMPTOM_H */
