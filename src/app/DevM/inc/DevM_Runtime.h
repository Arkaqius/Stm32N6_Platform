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

/* Macros and Defines -------------------------------------------------------*/
/* Typedefs -----------------------------------------------------------------*/
/* Exported Variables -------------------------------------------------------*/
/* Exported Interfaces ------------------------------------------------------*/
void DevM_MainFunction(void *params);
#endif /* DEVM_RUNTIME_H */
