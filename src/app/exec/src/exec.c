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

void Exec_InvokeRunnable(const ExecRunnable_T *runnable)
{
    // Exec_MeasurementStart(runnable);

    runnable->function(runnable->context);

    // Exec_MeasurementStop(runnable);
    // Exec_CheckDeadline(runnable);
}

void Exec_CreateTasks(const ExecTaskCfg_t *cfg)
{
    for (uint32_t idx = 0U; idx < cfg->size; idx++)
    {
        (void)xTaskCreateStatic(
            cfg->ar[idx].pxTaskCode,
            cfg->ar[idx].pcName,
            cfg->ar[idx].ulStackDepth,
            cfg->ar[idx].pvParameters,
            cfg->ar[idx].uxPriority,
            cfg->ar[idx].puxStackBuffer,
            cfg->ar[idx].pxTaskBuffer);
    }
}

/* Private Functions Implementation -----------------------------------------*/
