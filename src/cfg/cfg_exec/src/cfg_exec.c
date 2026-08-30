/**
 * @file cfg_exec.c
 * @brief Application configuration for the execution component
 */

/* Includes -----------------------------------------------------------------*/
#include "cfg_exec.h"
#include "exec_cfg.h"

_Static_assert(1, "Exec configuration is valid");

/* Defines ------------------------------------------------------------------*/

/* Local Types and Typedefs -------------------------------------------------*/

/* Global Variables ---------------------------------------------------------*/

/* Private Function Prototypes ----------------------------------------------*/

/* Public Functions Implementation ------------------------------------------*/

/* Private Functions Implementation -----------------------------------------*/
static void AsyncRunnable_Task1(void *params)
{
    void *pvParameters = params;

    uint32_t receivedEvent;

    for (;;)
    {
        if (xQueueReceive(devmEventQueue, &receivedEvent, portMAX_DELAY) == pdPASS)
        {
            /* Process the event through the state machine */
            DevM_RunStateMachine();
        }
        
    }
}

static void CyclicRunnable_Task1(void *pvParameters)
{
    (void)pvParameters;

    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        ExecInvokeRunnnable(FltMan_Runnable);
        
        FltMan_Tick(&g_flt_man);
        g_testFaultActive = FltMan_IsAnyFaultActive(&g_flt_man);

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(FLT_MAN_TASK_PERIOD_MS));
    }
}