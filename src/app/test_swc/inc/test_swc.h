/**
 * @file test_swc.h
 * @brief Interface for the demonstration Test Software Component
 *
 * This module periodically transmits a short message using the logger and
 * UART DMA drivers.  It serves as a template for new application software
 * components and showcases integration of the middleware modules.
 */

#ifndef TEST_SWC_H
#define TEST_SWC_H

/* Includes -----------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"

/* Macros and Defines -------------------------------------------------------*/

/* Typedefs -----------------------------------------------------------------*/

/* Exported Functions -------------------------------------------------------*/
/**
 * @brief Initialize the Test Software Component.
 *
 * The routine sets up peripheral dependencies and spawns the periodic
 * demonstration task.  It should be called once during system start-up
 * after the middleware drivers have been initialized.
 */
void TestSWC_Init(void);

#endif /* TEST_SWC_H */
