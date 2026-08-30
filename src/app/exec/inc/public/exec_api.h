/**
 * @file exec_api.h
 * @brief Public interface for the application execution component
 *
 * Provides initialization services for the application execution component.
 */

#ifndef EXEC_API_H
#define EXEC_API_H

/* Includes -----------------------------------------------------------------*/
#include "exec_cfg.h"

/* Macros and Defines -------------------------------------------------------*/

/* Typedefs -----------------------------------------------------------------*/

/* Exported Variables -------------------------------------------------------*/

/* Exported Interfaces ------------------------------------------------------*/
/**
 * @brief Initialize the application execution component.
 *
 * This function should be called once during application start-up.
 */
void Exec_InvokeRunnable(const ExecRunnable_T *runnable);

#endif /* EXEC_API_H */
