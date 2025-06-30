#include "symptom.h"

void Symptom_Set(Symptom_T *s)
{
    if (s) {
        s->active = true;
    }
}

void Symptom_Clear(Symptom_T *s)
{
    if (s) {
        s->active = false;
    }
}

bool Symptom_IsActive(const Symptom_T *s)
{
    return s ? s->active : false;
}
