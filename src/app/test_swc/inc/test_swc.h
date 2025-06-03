/**
 * @file test_swc.h
 * @brief Header file for Test Software Component
 *
 * This file provides the interface for initializing the Test SWC.
 */

#ifndef TEST_SWC_H
#define TEST_SWC_H

/* Includes -----------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"

/* Exported Functions -------------------------------------------------------*/
/**
 * @brief Initialize the Test Software Component
 *
 * This function creates the FreeRTOS task for the Test SWC.
 */
void TestSWC_Init(void);

#endif /* TEST_SWC_H */
