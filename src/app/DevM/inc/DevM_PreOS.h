/**
 * @file DevM_PreOS.h
 * @brief Header file for Pre-OS initialization functions in the Device Manager module.
 *
 * This file contains the declarations of functions responsible for initializing
 * various components of the system before the operating system is fully operational.
 * It includes initialization of infrastructure, middleware, services, and the OS itself.
 *
 * The functions defined here are part of the Device Manager module and are used
 * to ensure that all necessary components are properly set up during the early
 * stages of system startup.
 */

#ifndef DEVM_PREOS_H
#define DEVM_PREOS_H

/* Includes -----------------------------------------------------------------*/
#include "DevM.h"
#include "FreeRTOS.h"
#include "queue.h"

/* Macros and Defines -------------------------------------------------------*/
/* Typedefs -----------------------------------------------------------------*/
/* Exported Variables -------------------------------------------------------*/
extern QueueHandle_t devmEventQueue; /* TODO */
/* Exported Interfaces ------------------------------------------------------*/
DevM_ReturnType DevM_StateInitPreOS(void);
DevM_ReturnType DevM_StateInitOS(void);
#endif /* DEVM_PREOS_H */
