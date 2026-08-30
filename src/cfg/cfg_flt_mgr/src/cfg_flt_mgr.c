/**
 * @file cfg_flt_mgr.c
 * @brief Application configuration for the Fault Manager
 *
 * Defines the configured software symptoms, the aggregate software fault,
 * and all runtime storage owned by the configuration layer.
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>
#include "cfg_flt_mgr.h"
#include "fault_manager_types.h"
#include "fault_manager_fault_types.h"
#include "fault_manager_symptom_types.h"

/* Defines ------------------------------------------------------------------*/
#define CFG_FLT_SW_INPUT_COUNT ((uint8_t)(SYMPTOM_TOTAL - SYMPTOM_SW_NULL_PTR))
#define CFG_SYMPTOM_INIT(symptom_)           \
    [symptom_] = {                           \
        .cfg = &g_symptom_cfg[symptom_],     \
        .state = &g_symptom_state[symptom_], \
    }

/* Local Types and Typedefs -------------------------------------------------*/

/* Global Variables ---------------------------------------------------------*/
// clang-format off
static const SymptomCfg_T g_symptom_cfg[SYMPTOM_TOTAL] = {
    [SYMPTOM_INVALID            ] = {.id = SYMPTOM_INVALID            },
    [SYMPTOM_SW_NULL_PTR        ] = {.id = SYMPTOM_SW_NULL_PTR        },
    [SYMPTOM_SW_INVALID_ARGUMENT] = {.id = SYMPTOM_SW_INVALID_ARGUMENT},
    [SYMPTOM_SW_OUT_OF_RANGE    ] = {.id = SYMPTOM_SW_OUT_OF_RANGE    },
    [SYMPTOM_SW_INVALID_STATE   ] = {.id = SYMPTOM_SW_INVALID_STATE   },
    [SYMPTOM_SW_CONFIG_ERROR    ] = {.id = SYMPTOM_SW_CONFIG_ERROR    },
    [SYMPTOM_SW_INVALID_RESOURCE] = {.id = SYMPTOM_SW_INVALID_RESOURCE},
    [SYMPTOM_SW_INIT_FAILURE    ] = {.id = SYMPTOM_SW_INIT_FAILURE    },
    [SYMPTOM_SW_TIMEOUT         ] = {.id = SYMPTOM_SW_TIMEOUT         },
    [SYMPTOM_SW_BUFFER_OVERFLOW ] = {.id = SYMPTOM_SW_BUFFER_OVERFLOW },
    [SYMPTOM_SW_ASSERT_FAILURE  ] = {.id = SYMPTOM_SW_ASSERT_FAILURE  },
};

static SymptomState_T g_symptom_state[SYMPTOM_TOTAL] = {0};

static const Symptom_T g_symptoms[SYMPTOM_TOTAL] = {
    CFG_SYMPTOM_INIT(SYMPTOM_INVALID),
    CFG_SYMPTOM_INIT(SYMPTOM_SW_NULL_PTR),
    CFG_SYMPTOM_INIT(SYMPTOM_SW_INVALID_ARGUMENT),
    CFG_SYMPTOM_INIT(SYMPTOM_SW_OUT_OF_RANGE),
    CFG_SYMPTOM_INIT(SYMPTOM_SW_INVALID_STATE),
    CFG_SYMPTOM_INIT(SYMPTOM_SW_CONFIG_ERROR),
    CFG_SYMPTOM_INIT(SYMPTOM_SW_INVALID_RESOURCE),
    CFG_SYMPTOM_INIT(SYMPTOM_SW_INIT_FAILURE),
    CFG_SYMPTOM_INIT(SYMPTOM_SW_TIMEOUT),
    CFG_SYMPTOM_INIT(SYMPTOM_SW_BUFFER_OVERFLOW),
    CFG_SYMPTOM_INIT(SYMPTOM_SW_ASSERT_FAILURE),
};

static const FaultCfg_T g_fault_cfg[FAULT_TOTAL] = {
    [FAULT_INVALID] = {0},
    [FAULT_SW_ERROR] = {
        .inputs = &g_symptoms[SYMPTOM_SW_NULL_PTR],
        .input_count = CFG_FLT_SW_INPUT_COUNT,
        .threshold = 1U,
        .window = 1U,
        .shadow_faults = NULL,
        .shadow_count = 0U,
        .on_transition = NULL,
        .capture_freeze_frame = NULL,
    },
};

static FaultRuntime_T g_fault_runtime[FAULT_TOTAL] = {
    [FAULT_INVALID] = {0},
    [FAULT_SW_ERROR] = {
        .state = FAULT_STATE_INACTIVE,
        .counter = 0U,
        .inhibit = false,
    },
};

static const Flt_T g_faults[FAULT_TOTAL] = {
    [FAULT_INVALID] = {0},
    [FAULT_SW_ERROR] = {
        .cfg = &g_fault_cfg[FAULT_SW_ERROR],
        .rt = &g_fault_runtime[FAULT_SW_ERROR],
    },
};

static const FltMan_Cfg_T g_flt_man_cfg = {
    .faults = g_faults,
    .symptoms = g_symptoms,
    .fault_count = FAULT_TOTAL,
    .symptom_count = SYMPTOM_TOTAL,
};

static FltMan_Runtime_T g_flt_man_runtime = {
    .counter = 0U,
};

const FltMan_T g_flt_man = {
    .cfg = &g_flt_man_cfg,
    .runtime = &g_flt_man_runtime,
};
// clang-format on

/* Private Function Prototypes ----------------------------------------------*/

/* Public Functions Implementation ------------------------------------------*/

/* Private Functions Implementation -----------------------------------------*/
