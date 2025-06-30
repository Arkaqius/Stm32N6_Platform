#ifndef FAULT_MANAGER_H
#define FAULT_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "fault.h"

typedef struct {
    Fault_T *const *faults;
    uint8_t fault_count;
} FaultManager_T;

void FaultManager_Tick(FaultManager_T *mgr);
bool FaultManager_IsAnyFaultActive(FaultManager_T *mgr);
void FaultManager_ForceAllClear(FaultManager_T *mgr);

#endif /* FAULT_MANAGER_H */
