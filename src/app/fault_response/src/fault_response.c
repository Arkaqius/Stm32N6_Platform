#include "fault_response.h"

typedef struct {
    Fault_T *fault;
    FaultResponseHook hook;
} FaultResponseEntry;

static FaultResponseEntry response_table[FAULT_RESPONSE_MANAGER_MAX_ENTRIES];
static uint8_t response_count = 0;

void FaultResponseManager_Register(Fault_T *fault, FaultResponseHook hook)
{
    if (response_count >= FAULT_RESPONSE_MANAGER_MAX_ENTRIES) {
        return;
    }

    response_table[response_count].fault = fault;
    response_table[response_count].hook = hook;
    response_count++;
}

void FaultResponseManager_Dispatch(Fault_T *fault, bool new_state)
{
    for (uint8_t i = 0; i < response_count; ++i) {
        if (response_table[i].fault == fault && response_table[i].hook) {
            response_table[i].hook(fault, new_state);
            break;
        }
    }
}
