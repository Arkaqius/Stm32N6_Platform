/**
 * @file test_swc.c
 * @brief Source file for Test Software Component
 *
 * This file implements the initialization and task handling for the Test SWC.
 */

/* Includes -----------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "test_swc.h"
#include "UartDma.h" // Include UART DMA header for testing
#include "logger.h"  // Include logger for logging messages
#include "SysM.h"    // Include System Manager API for system services
#include <string.h>  // For string operations

/* Defines ------------------------------------------------------------------*/
#define TEST_TASK_PERIOD_MS 1 /**< Period of the demo task in milliseconds */

/** Test message sent over UART DMA for demonstration purposes. */
static char testMessage[] = "Hello\r\n";
/* Private Function Prototypes ----------------------------------------------*/
static void TestTask(void *pvParameters);

/* Public Functions Implementation ------------------------------------------*/
void TestSWC_Init(void)
{
    // Initialize the UART DMA module
    UartDma_Init();

    // Create the FreeRTOS task for Test SWC
    xTaskCreate(TestTask, "TestTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
}

/* Private Functions Implementation -----------------------------------------*/
/**
 * @brief Task function for Test SWC
 *
 * This task runs periodically every 100ms.
 *
 * @param[in] pvParameters Task parameters (not used)
 */
static void TestTask(void *pvParameters)
{
    (void)pvParameters;
    static Logger_Entry_T full_entry = {.msg = "Queue FULL\r\n", .length = 16};

    TickType_t xLastWakeTime = xTaskGetTickCount();
    Logger_Context_T *loggerCtx = Cfg_Logger_GetContext();
    logger_register_highprio(loggerCtx, 0, &full_entry); // Register a high-priority log entry

    for (;;)
    {
        Logger_Entry_T *entry = logger_alloc_entry(loggerCtx); // Allocate a log entry
        if (entry)
        {
            strcpy((char *)entry->msg, testMessage); // Copy the test message into the log entry
            entry->length = sizeof(testMessage) - 1; // Set the length of the message

            // logger_debug_push(loggerCtx, xTaskGetTickCount()); // Push the current tick count to the debug buffer
            logger_commit_entry(loggerCtx, entry);                      // Commit the log entry for transmission
            logger_trigger_highprio(loggerCtx, 0, xTaskGetTickCount()); // Trigger high-priority log if allocation fails
        }
        else
        {
            logger_trigger_highprio(loggerCtx, 0, xTaskGetTickCount()); // Trigger high-priority log if allocation fails
        }
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(TEST_TASK_PERIOD_MS));
    }
}