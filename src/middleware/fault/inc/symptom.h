#ifndef SYMPTOM_H
#define SYMPTOM_H

#include <stdbool.h>

typedef struct {
    bool active;
} Symptom_T;

void Symptom_Set(Symptom_T *s);
void Symptom_Clear(Symptom_T *s);
bool Symptom_IsActive(const Symptom_T *s);

#endif /* SYMPTOM_H */
