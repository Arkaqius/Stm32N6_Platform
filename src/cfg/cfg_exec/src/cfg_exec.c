/**
 * @file cfg_exec.c
 * @brief Application configuration for the execution component
 */

/* Includes -----------------------------------------------------------------*/
#include "cfg_exec.h"
#include "exec_cfg.h"
#include "FreeRTOS.h"
#include "fault_manager_api.h"
#include "cfg_flt_mgr.h"
#include "exec_api.h"

_Static_assert(1, "Exec configuration is valid");

/* Defines ------------------------------------------------------------------*/
#define TASK1 (0)
#define EXEC_TASK_1_STACK_SIZE (512U)
/* Local Types and Typedefs -------------------------------------------------*/

/* Global Variables ---------------------------------------------------------*/
static void CyclicRunnable50ms_Task1(void *pvParameters);

static ExecRunnable_T FltMangRunnable = {
    .context = &g_flt_man,
    .function = FltMan_Tick,
    .name = "FltManTick",
    .period_ms = 50,
};

static StackType_t g_task1Stack[EXEC_TASK_1_STACK_SIZE];
static StaticTask_t g_task1ControlBlock;

static const ExecTaskEntry_t g_execTaskEntries[] =
    {
        [TASK1] = {
            .pxTaskCode = CyclicRunnable50ms_Task1,
            .pcName = "CyclicRunnable50ms_Task1",
            .ulStackDepth = EXEC_TASK_1_STACK_SIZE,
            .pvParameters = NULL,
            .uxPriority = tskIDLE_PRIORITY + 2U,
            .puxStackBuffer = g_task1Stack,
            .pxTaskBuffer = &g_task1ControlBlock,
        },
};

const ExecTaskCfg_t g_execTasksCfg =
    {
        .ar = g_execTaskEntries,
        .size = ARRAY_SIZE(g_execTaskEntries),
};
/* Private Function Prototypes ----------------------------------------------*/

/* Public Functions Implementation ------------------------------------------*/

/* Private Functions Implementation -----------------------------------------*/
static void CyclicRunnable50ms_Task1(void *pvParameters)
{
    (void)pvParameters;

    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        Exec_InvokeRunnable(&FltMangRunnable);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50));
    }
}