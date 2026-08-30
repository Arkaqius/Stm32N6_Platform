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
/* Macros and Defines -------------------------------------------------------*/

/* Typedefs -----------------------------------------------------------------*/
typedef void (*ExecRunnableFn)(void *context);

typedef struct ExecRunnable_Tag
{
    const char     *name;
    ExecRunnableFn function;
    void           *context;
    uint32_t        period_ticks;
    uint32_t        phase_ticks;
} ExecRunnable_T;
/* Exported Variables -------------------------------------------------------*/

/* Exported Interfaces ------------------------------------------------------*/

#endif /* EXEC_CFG_H */
