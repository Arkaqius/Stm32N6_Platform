#ifndef FAULT_H
#define FAULT_H

#include <stdbool.h>
#include <stdint.h>

struct Fault_T;

typedef void (*FaultTransitionHook)(struct Fault_T *fault, bool new_state);
typedef void (*FaultFreezeFrameHook)(struct Fault_T *fault);

typedef enum {
    FAULT_STATE_INACTIVE = 0,
    FAULT_STATE_ACTIVE
} FaultState_T;

typedef struct Fault_T {
    FaultState_T state;
    const bool *const *inputs;
    uint8_t input_count;
    uint8_t counter;
    uint8_t threshold;
    uint8_t window;
    bool inhibit;
    const struct Fault_T *const *shadow_faults;
    uint8_t shadow_count;
    FaultTransitionHook on_transition;
    FaultFreezeFrameHook capture_freeze_frame;
} Fault_T;

void Fault_Tick(Fault_T *f);
bool Fault_IsActive(const Fault_T *f);
void Fault_SetInhibit(Fault_T *f, bool enabled);
void Fault_ForceState(Fault_T *f, bool state);

#endif /* FAULT_H */
