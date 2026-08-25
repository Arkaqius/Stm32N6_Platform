/**
 * @file DevM_Runtime.h
 * @brief Header file for runtime state functions in the Device Manager module.
 *
 * This file contains the declarations of functions responsible for managing
 * the runtime states of the system, including normal operation, fault handling,
 * and software restart procedures.
 *
 * The functions defined here are part of the Device Manager module and are used
 * to ensure proper state transitions and handling during the runtime phase of
 * the system's operation.
 */

#ifndef DEVM_RUNTIME_H
#define DEVM_RUNTIME_H
/* Includes -----------------------------------------------------------------*/
#include "DevM.h"
#include "logger.h"

/* Macros and Defines -------------------------------------------------------*/
/* Typedefs -----------------------------------------------------------------*/
/* Exported Variables -------------------------------------------------------*/
/* Exported Interfaces ------------------------------------------------------*/
/**
 * @brief Execute the state machine task loop.
 *
 * The function waits for events on the Device Manager queue and processes
 * them through the state machine.
 *
 * @param params Unused parameter required by FreeRTOS task signature.
 */
void DevM_MainFunction(void *params);

/**
 * @brief Obtain the global logger context used by the application.
 *
 * @return Pointer to the statically allocated Logger_Context_T instance.
 */
Logger_Context_T *Cfg_Logger_GetContext(void);

/**
 * @brief Initialize the application logger and register static messages.
 */
void Cfg_Logger_Init(void);
#endif /* DEVM_RUNTIME_H */
