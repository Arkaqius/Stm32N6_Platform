#ifndef FAULT_RESPONSE_H
#define FAULT_RESPONSE_H

#include "fault.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*FaultResponseHook)(Fault_T *fault, bool active);

#ifndef FAULT_RESPONSE_MANAGER_MAX_ENTRIES
#define FAULT_RESPONSE_MANAGER_MAX_ENTRIES 16
#endif

void FaultResponseManager_Register(Fault_T *fault, FaultResponseHook hook);
void FaultResponseManager_Dispatch(Fault_T *fault, bool new_state);

#ifdef __cplusplus
}
#endif

#endif /* FAULT_RESPONSE_H */
