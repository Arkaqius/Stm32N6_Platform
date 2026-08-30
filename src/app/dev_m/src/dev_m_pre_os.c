/**
 * @file dev_m_pre_os.c
 * @brief Device Manager Pre-OS Initialization Module
 *
 * This file contains the implementation of the Device Manager's
 * pre-OS initialization functions, including infrastructure setup,
 * basic software initialization, middleware initialization, and
 * service initialization.
 */

/* Includes -----------------------------------------------------------------*/
#include "stm32n657xx.h"
#include "dev_m.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "core_cm55.h"
#include "cmsis_gcc.h"
#include "dev_m_runtime.h"
#include "test_swc.h" // Include the header for Test SWC
#include "stm32n6xx_ll_bus.h"
#include "stm32n6xx_ll_pwr.h"
#include "stm32n6xx_ll_rcc.h"
#include "stm32n6xx_ll_utils.h"
/* Logger */
#include "logger.h"     /* Logger API */
#include "cfg_logger.h" /* App cfg */
#include "cfg_exec.h"
/* Defines ------------------------------------------------------------------*/

/* Local Types and Typedefs -------------------------------------------------*/

/* Global Variables ---------------------------------------------------------*/
/**< Handle of the Device Manager main task created during OS init. */
static TaskHandle_t devmTaskHandle = NULL;

/** Queue used to send events to the Device Manager state machine. */
/* Private Function Prototypes ----------------------------------------------*/
QueueHandle_t devmEventQueue = NULL;
/** Initialize basic infrastructure prior to OS start. */
static DevM_ReturnType DevM_InitInfra(void);
/** Initialize basic software components before OS start. */
static DevM_ReturnType DevM_StateInitBswPreOS(void);
/** Initialize middleware components before OS start. */
static DevM_ReturnType DevM_StateInitMiddlewarePreOS(void);
/** Initialize service components before OS start. */
static DevM_ReturnType DevM_StateInitAppPreOS(void);
/** Disable resource security to allow full SRAM access. */
static void DevM_DisableResourceSecurity(void);

static void SystemClock_Config(void);
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
    if (DevM_StateInitAppPreOS() != DEVM_OK)
        return DEVM_ERROR;
    return DEVM_OK;
}

/* Implementation of OS initialization function */
DevM_ReturnType DevM_StateInitOS(void)
{
    Exec_CreateTasks(&g_execTasksCfg);

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
/**
 * @brief Initialize infrastructure components such as caches and clocks.
 */
static DevM_ReturnType DevM_InitInfra(void)
{
    SCB_EnableICache();
    SCB_EnableDCache();

    /* Allow all master to AXI SRAM1/2 */
    DevM_DisableResourceSecurity();

    /* Configure priority grouping */
    NVIC_SetPriorityGrouping(4U);

    /* Configure the power domain */
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_PWR);
    LL_PWR_ConfigSupply(LL_PWR_EXTERNAL_SOURCE_SUPPLY);
    while (LL_PWR_IsActiveFlag_ACTVOSRDY() == 0U)
    {
    }

    /* Config clock tree */
    SystemClock_Config();

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

void SystemClock_Config(void)
{
    /* Configure the System Power Supply */
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_PWR);
    LL_PWR_ConfigSupply(LL_PWR_EXTERNAL_SOURCE_SUPPLY);
    while (LL_PWR_IsActiveFlag_ACTVOSRDY() == 0U)
    {
    }

    LL_RCC_HSI_Enable();

    /* Wait till HSI is ready */
    while (LL_RCC_HSI_IsReady() == 0)
    {
    }

    /** Get current CPU/System buses clocks configuration and
     *if necessary switch to intermediate HSI clock to ensure target clock can be set
     */
    if ((LL_RCC_GetCpuClkSource() == LL_RCC_CPU_CLKSOURCE_STATUS_IC1) ||
        (LL_RCC_GetSysClkSource() == LL_RCC_SYS_CLKSOURCE_STATUS_IC2_IC6_IC11))
    {
        LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
        while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
        {
        }
        LL_RCC_SetCpuClkSource(LL_RCC_CPU_CLKSOURCE_HSI);
        while (LL_RCC_GetCpuClkSource() != LL_RCC_CPU_CLKSOURCE_STATUS_HSI)
        {
        }
    }
    LL_RCC_PLL1_Disable();
    while (LL_RCC_PLL1_IsReady() == 1)
    {
    }
    LL_RCC_PLL1_DisableModulationSpreadSpectrum();
    LL_RCC_PLL1_DisableBypass();
    LL_RCC_PLL1_SetSource(LL_RCC_PLLSOURCE_HSI);
    LL_RCC_PLL1_SetM(4);
    LL_RCC_PLL1_SetN(75);
    LL_RCC_PLL1_SetP1(1);
    LL_RCC_PLL1_SetP2(1);
    LL_RCC_PLL1_SetFRACN(0);
    LL_RCC_PLL1_AssertModulationSpreadSpectrumReset();
    LL_RCC_PLL1_DisableFractionalModulationSpreadSpectrum();
    LL_RCC_PLL1P_Enable();
    LL_RCC_PLL1_Enable();
    while (LL_RCC_PLL1_IsReady() != 1)
    {
    }

    LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
    LL_RCC_SetAPB4Prescaler(LL_RCC_APB4_DIV_1);
    LL_RCC_SetAPB5Prescaler(LL_RCC_APB5_DIV_1);

    LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_1);

    LL_RCC_IC1_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
    LL_RCC_IC1_SetDivider(2);
    LL_RCC_IC1_Enable();
    LL_RCC_SetCpuClkSource(LL_RCC_CPU_CLKSOURCE_IC1);
    while (LL_RCC_GetCpuClkSource() != LL_RCC_CPU_CLKSOURCE_STATUS_IC1)
    {
    }

    LL_RCC_IC2_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
    LL_RCC_IC2_SetDivider(3);
    LL_RCC_IC6_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
    LL_RCC_IC6_SetDivider(4);
    LL_RCC_IC11_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
    LL_RCC_IC11_SetDivider(3);
    LL_RCC_IC2_Enable();
    LL_RCC_IC6_Enable();
    LL_RCC_IC11_Enable();
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_IC2_IC6_IC11);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_IC2_IC6_IC11)
    {
    }
    LL_SetSystemCoreClock(600000000);
}

/**
 * @brief Initialize basic software modules before OS start.
 */
static DevM_ReturnType DevM_StateInitBswPreOS(void)
{

    return DEVM_OK;
}
/**
 * @brief Initialize middleware components before OS start.
 */
static DevM_ReturnType DevM_StateInitMiddlewarePreOS(void)
{
    Cfg_Logger_Init();
    return DEVM_OK;
}
/**
 * @brief Initialize application services before OS start.
 */
static DevM_ReturnType DevM_StateInitAppPreOS(void)
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
