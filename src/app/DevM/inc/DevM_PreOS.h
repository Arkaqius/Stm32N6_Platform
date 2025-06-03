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
/**
 * @brief Queue used to post events to the Device Manager state machine.
 */
extern QueueHandle_t devmEventQueue;

/* Exported Interfaces ------------------------------------------------------*/
/**
 * @brief Perform the pre-operating-system initialization sequence.
 *
 * This includes infrastructure, basic software, middleware and service setup
 * required before the scheduler is started.
 *
 * @return Result of the initialization step.
 */
DevM_ReturnType DevM_StateInitPreOS(void);

/**
 * @brief Perform initialization that requires the operating system to be
 * running.
 *
 * OS resources such as tasks and queues are created during this stage.
 *
 * @return Result of the initialization step.
 */
DevM_ReturnType DevM_StateInitOS(void);
#endif /* DEVM_PREOS_H */
