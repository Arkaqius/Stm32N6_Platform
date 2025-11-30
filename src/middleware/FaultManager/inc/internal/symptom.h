#ifndef SYMPTOM_H
#define SYMPTOM_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint32_t id;
} SymptomCfg_T;

typedef struct
{
    volatile bool isActive;
    volatile uint32_t last_update_tick;
} SymptomState_T;

typedef struct
{
    const SymptomCfg_T *cfg; /* ROM */
    SymptomState_T *state;   /* RAM */
} Symptom_T;

/* Compile-time initializer for RAM state */
#define SYMPTOM_RUNTIME_INIT \
    {.isActive = false, .last_update_tick = 0u}

/* Primitive ops work on the STATE (keeps this layer tiny) */
void Symptom_SetLevel(SymptomState_T *s, bool active, uint32_t tick);
static inline bool Symptom_IsActive(const SymptomState_T *s) { return s ? s->isActive : false; }

#endif
