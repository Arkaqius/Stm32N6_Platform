/**
 * @file test_swc.c
 * @brief Implementation of the Test Software Component
 *
 * The Test SWC periodically allocates a log entry, fills it with a fixed
 * string and commits it for transmission over the UART DMA driver.  It
 * demonstrates how the logger can be used from an application task.
 */

/* Includes -----------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "test_swc.h"
#include "UartDma.h"    // Include UART DMA header for testing
#include "logger.h"     // Include logger for logging messages
#include "cfg_logger.h" // Logger configuration
#include "SysM.h"       // Include System Manager API for system services
#include <string.h>     // For string operations

/* Defines ------------------------------------------------------------------*/
#define TEST_TASK_PERIOD_MS (1U) /**< Period of the demo task in milliseconds */

/* Local Types and Typedefs -------------------------------------------------*/

/* Global Variables ---------------------------------------------------------*/
/** Test message sent over UART DMA for demonstration purposes. */
static char testMessage[] = "Hello\r\n";

/* Private Function Prototypes ----------------------------------------------*/
static void TestTask(void *pvParameters);

/* Public Functions Implementation ------------------------------------------*/
/**
 * @brief Initialize the Test Software Component.
 *
 * Sets up the UART DMA driver and creates the FreeRTOS task that
 * periodically logs a demo string.  Should be called once during
 * application start-up.
 */
void TestSWC_Init(void)
{
    // Initialize the UART DMA module
    UartDma_Init();

    // Create the FreeRTOS task for Test SWC
    xTaskCreate(TestTask, "TestTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
}

/* Private Functions Implementation -----------------------------------------*/
/**
 * @brief Periodic task demonstrating logger usage.
 *
 * Allocates a log entry every cycle, copies the demo string into the
 * entry buffer and commits it for asynchronous transmission.  When no
 * entry is available the high priority "allocation failed" message is
 * triggered instead.
 *
 * @param[in] pvParameters Unused task parameter.
 */
static void TestTask(void *pvParameters)
{
    (void)pvParameters;

    TickType_t xLastWakeTime = xTaskGetTickCount();
    Logger_Context_T *loggerCtx = Cfg_Logger_GetContext();

    for (;;)
    {
        Logger_Entry_T *entry = logger_alloc_entry(loggerCtx); // Allocate a log entry
        if (entry)
        {
            strcpy((char *)entry->msg, testMessage); // Copy the test message into the log entry
            entry->length = sizeof(testMessage) - 1; // Set the length of the message
            logger_commit_entry(loggerCtx, entry);   // Commit the log entry for transmission
        }
        else
        {
            logger_trigger_highprio(loggerCtx, CFG_LOGGER_ALLOC_FAILED, xTaskGetTickCount());
        }
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(TEST_TASK_PERIOD_MS));
    }
}
