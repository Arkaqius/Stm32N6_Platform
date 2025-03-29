/**
 * @file DevM_PreOS.c
 * @brief Device Manager Pre-OS Initialization Module
 *
 * This file contains the implementation of the Device Manager's
 * pre-OS initialization functions, including infrastructure setup,
 * basic software initialization, middleware initialization, and
 * service initialization.
 */

/* Includes -----------------------------------------------------------------*/
#include "stm32n657xx.h"
#include "DevM.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "core_cm55.h"
#include "cmsis_gcc.h"
#include "DevM_Runtime.h"

/* Defines ------------------------------------------------------------------*/

/* Local Types and Typedefs -------------------------------------------------*/

/* Global Variables ---------------------------------------------------------*/
static TaskHandle_t devmTaskHandle = NULL;
/* Queue handle for state events */
QueueHandle_t devmEventQueue = NULL; /* TODO */
/* Private Function Prototypes ----------------------------------------------*/
static DevM_ReturnType DevM_InitInfra(void);
static DevM_ReturnType DevM_StateInitBswPreOS(void);
static DevM_ReturnType DevM_StateInitMiddlewarePreOS(void);
static DevM_ReturnType DevM_StateInitServicesPreOS(void);
/* Public Functions Implementation ------------------------------------------*/
/**
 * @brief Initialize all pre-OS components.
 * @return DEVM_OK if successful.
 */
DevM_ReturnType DevM_StateInitPreOS(void)
{
    if (DevM_InitInfra() != DEVM_OK)
        return DEVM_ERROR;
    if (DevM_StateInitBswPreOS() != DEVM_OK)
        return DEVM_ERROR;
    if (DevM_StateInitMiddlewarePreOS() != DEVM_OK)
        return DEVM_ERROR;
    if (DevM_StateInitServicesPreOS() != DEVM_OK)
        return DEVM_ERROR;
    return DEVM_OK;
}

/* Implementation of OS initialization function */
DevM_ReturnType DevM_StateInitOS(void)
{
    /* Create queue for state events */
    devmEventQueue = xQueueCreate(10, sizeof(uint32_t));
    if (devmEventQueue == NULL)
    {
        return DEVM_ERROR;
    }

    /* Create Task for DevM State Run */
    BaseType_t taskCreated = xTaskCreate(
        DevM_MainFunction,
        "DevM_Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        &devmTaskHandle);

    return (taskCreated == pdPASS) ? DEVM_OK : DEVM_ERROR;
}

/* Private Functions Implementation -----------------------------------------*/
static DevM_ReturnType DevM_InitInfra(void)
{
    SCB_EnableICache();
    SCB_EnableDCache();

    /* Configure priority grouping */
    NVIC_SetPriorityGrouping(4U);

    /* Update SystemCoreClock variable */
    SystemCoreClockUpdate();

    /* Configure SysTick to generate interrupt for FreeRTOS tick */
    if (SysTick_Config(SystemCoreClock / configTICK_RATE_HZ) != 0)
    {
        // Configuration failed
        while (1)
            ;
    }

    /* Set SysTick Interrupt priority */
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),
                                                       configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0));

    return DEVM_OK;
}

static DevM_ReturnType DevM_StateInitBswPreOS(void) { return DEVM_OK; }
static DevM_ReturnType DevM_StateInitMiddlewarePreOS(void) { return DEVM_OK; }
static DevM_ReturnType DevM_StateInitServicesPreOS(void) { return DEVM_OK; }