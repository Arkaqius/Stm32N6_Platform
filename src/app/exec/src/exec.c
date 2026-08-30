/**
 * @file exec.c
 * @brief Implementation of the application execution component
 */

/* Includes -----------------------------------------------------------------*/
#include "exec_api.h"
#include "exec_cfg.h"
#include "FreeRTOS.h"
/* Defines ------------------------------------------------------------------*/

/* Local Types and Typedefs -------------------------------------------------*/

/* Global Variables ---------------------------------------------------------*/

/* Private Function Prototypes ----------------------------------------------*/

/* Public Functions Implementation ------------------------------------------*/

void Exec_Invoke(const ExecRunnable_T *runnable)
{
    Exec_MeasurementStart(runnable);

    runnable->function(runnable->context);

    Exec_MeasurementStop(runnable);
    Exec_CheckDeadline(runnable);
}

void Exec_CreateTasks(ExecTaskCfg_t cfg[])
{
    for (uint32_t idx = 0; idx < MAX_TASK; idx++)
    {
        xTaskCreateStatic(cfg[idx].pxTaskCode,
                                    cfg[idx].pcName,
                                    cfg[idx].ulStackDepth,
                                    cfg[idx].pvParameters,
                                    cfg[idx].uxPriority,
                                    cfg[idx].puxStackBuffer,
                                    cfg[idx].pxTaskBuffer );
    }
    
}

/* Private Functions Implementation -----------------------------------------*/
