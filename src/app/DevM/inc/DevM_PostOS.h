/**
 * @file DevM_PostOS.h
 * @brief Header file for Post-OS initialization functions in the Device Manager module.
 *
 * This file contains the declarations of functions responsible for initializing
 * various components of the system after the operating system is fully operational.
 * It includes initialization of middleware, services, and application-specific components.
 *
 * The functions defined here are part of the Device Manager module and are used
 * to ensure that all necessary components are properly set up during the post-OS
 * initialization phase of system startup.
 */

#ifndef DEVM_POSTOS_H
#define DEVM_POSTOS_H

/* Includes -----------------------------------------------------------------*/
#include "DevM.h"

/* Macros and Defines -------------------------------------------------------*/
/* Typedefs -----------------------------------------------------------------*/
/* Exported Variables -------------------------------------------------------*/
/* Exported Interfaces ------------------------------------------------------*/
/**
 * @brief Perform initialization steps that require the OS to be running.
 *
 * Middleware and application level components are initialized during this
 * stage once scheduler is active.
 *
 * @return Result of the initialization step.
 */
DevM_ReturnType DevM_StateInitPostOS(void);

#endif /* DEVM_POSTOS_H */
