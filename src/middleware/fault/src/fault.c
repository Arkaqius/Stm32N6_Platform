#include "fault.h"

void Fault_Tick(Fault_T *f)
{
    if (!f) {
        return;
    }

    bool input_active = false;
    for (uint8_t i = 0; i < f->input_count; ++i) {
        const bool *src = f->inputs[i];
        if (src && *src) {
            input_active = true;
            break;
        }
    }

    if (input_active) {
        if (f->window == 0 || f->counter < f->window) {
            if (f->counter < UINT8_MAX) {
                f->counter++;
            }
        }
    } else if (f->counter > 0) {
        f->counter--;
    }

    if (f->counter >= f->threshold) {
        f->state = FAULT_STATE_ACTIVE;
    } else if (f->counter == 0) {
        f->state = FAULT_STATE_INACTIVE;
    }
}

bool Fault_IsActive(const Fault_T *f)
{
    return f ? (f->state == FAULT_STATE_ACTIVE) : false;
}

void Fault_SetInhibit(Fault_T *f, bool enabled)
{
    if (f) {
        f->inhibit = enabled;
    }
}

void Fault_ForceState(Fault_T *f, bool state)
{
    if (!f) {
        return;
    }

    f->counter = state ? f->threshold : 0;
    f->state = state ? FAULT_STATE_ACTIVE : FAULT_STATE_INACTIVE;
}
