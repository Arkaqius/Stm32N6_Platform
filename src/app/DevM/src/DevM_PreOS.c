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
#include "test_swc.h" // Include the header for Test SWC
#include "SysM.h"     /* System Manager API */

/* Logger */
#include "logger.h"     /* Logger API */
#include "cfg_logger.h" /* App cfg */

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
static void DevM_DisableResourceSecurity(void);
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

    /* Create task for Logger */
    xTaskCreate(
        logger_tx_task,
        "logger_tx_task",
        configMINIMAL_STACK_SIZE,
        Cfg_Logger_GetContext(),
        tskIDLE_PRIORITY + 1,
        &(Cfg_Logger_GetContext()->logger_task_handle));

    return (taskCreated == pdPASS) ? DEVM_OK : DEVM_ERROR;
}

/* Private Functions Implementation -----------------------------------------*/
static DevM_ReturnType DevM_InitInfra(void)
{
    SCB_EnableICache();
    SCB_EnableDCache();

    /* Allow all master to AXI SRAM1/2 */
    DevM_DisableResourceSecurity();

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

static DevM_ReturnType DevM_StateInitBswPreOS(void)
{

    return DEVM_OK;
}
static DevM_ReturnType DevM_StateInitMiddlewarePreOS(void)
{

    return DEVM_OK;
}
static DevM_ReturnType DevM_StateInitServicesPreOS(void)
{
    TestSWC_Init();
    return DEVM_OK;
}

/**
 * @brief Disable resource security for RISAF2 and RISAF3.
 *
 * This function configures the RISAF2 and RISAF3 resource isolation
 * and security attributes, disabling security and enabling access
 * for all cores to the entire SRAM2/SRAM1 region.
 */
static void DevM_DisableResourceSecurity(void)
{
    /* Disable resource security for RISAF3 */
    RISAF3->REG[0].CFGR = 0x00000000;    // Reset first
    RISAF3->REG[0].CIDCFGR = 0x000F000F; // RW for all
    RISAF3->REG[0].ENDR = 0xFFFFFFFF;    // Entire SRAM2 region
    RISAF3->REG[0].CFGR = 0x00000101;    // Enable region, unprivileged, secure
    RISAF3->REG[1].CIDCFGR = 0x00FF00FF; // RW for all
    RISAF3->REG[1].ENDR = 0xFFFFFFFF;
    RISAF3->REG[1].CFGR = 0x00000001; // Enable region, non-secure, unprivileged

    /* Disable resource security for RISAF2 */
    RISAF2->REG[0].CFGR = 0x00000000;    // Reset first
    RISAF2->REG[0].CIDCFGR = 0x000F000F; // RW for all
    RISAF2->REG[0].ENDR = 0xFFFFFFFF;    // Entire SRAM2 region
    RISAF2->REG[0].CFGR = 0x00000101;    // Enable region, unprivileged, secure
    RISAF2->REG[1].CIDCFGR = 0x00FF00FF; // RW for all
    RISAF2->REG[1].ENDR = 0xFFFFFFFF;
    RISAF2->REG[1].CFGR = 0x00000001; // Enable region, non-secure, unprivileged
}