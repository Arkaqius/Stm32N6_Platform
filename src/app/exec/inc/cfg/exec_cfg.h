/**
 * @file exec_cfg.h
 * @brief Configuration interface for the application execution component
 *
 * Defines compile-time configuration values used by the execution component.
 */

#ifndef EXEC_CFG_H
#define EXEC_CFG_H

/* Includes -----------------------------------------------------------------*/
#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"
/* Macros and Defines -------------------------------------------------------*/
#define ARRAY_SIZE(array_) \
    (sizeof(array_) / sizeof((array_)[0]))

/* Typedefs -----------------------------------------------------------------*/
typedef void (*ExecRunnableFn)(void *context);

typedef struct ExecRunnable_Tag
{
    const char *name;
    ExecRunnableFn function;
    void *context;
    uint32_t period_ms;
} ExecRunnable_T;

typedef struct
{
    TaskFunction_t pxTaskCode;
    const char *pcName;
    configSTACK_DEPTH_TYPE ulStackDepth;
    void *pvParameters;
    UBaseType_t uxPriority;
    StackType_t *puxStackBuffer;
    StaticTask_t *pxTaskBuffer;
} ExecTaskEntry_t;

typedef struct
{
    const ExecTaskEntry_t *ar;
    uint32_t size;
} ExecTaskCfg_t;
/* Exported Variables -------------------------------------------------------*/

/* Exported Interfaces ------------------------------------------------------*/

#endif /* EXEC_CFG_H */
