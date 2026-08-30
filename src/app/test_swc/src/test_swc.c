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
#include "uart_dma.h"     // Include UART DMA header for testing
#include "logger.h"       // Include logger for logging messages
#include "cfg_logger.h"   // Logger configuration
#include "cfg_flt_mgr.h"  // Fault Manager configuration
#include "fault_manager_api.h" // Fault Manager public API
#include "dev_m_runtime.h" // Logger context provider
#include <string.h>       // For string operations
#include "stm32n6xx_ll_adc.h"
#include "stm32n6xx_ll_bus.h"
#include "stm32n6xx_ll_gpio.h"
#include "stm32n6xx_ll_system.h"
#include "stm32n6xx_ll_utils.h"
#include "stm32n6xx_ll_pwr.h"
/* Defines ------------------------------------------------------------------*/
#define TEST_TASK_PERIOD_MS (100U)   /**< Period of the demo task in milliseconds */
#define FLT_MAN_TASK_PERIOD_MS (10U) /**< Period of fault evaluation in milliseconds. */

/* ADC Test */
#define ADCx ADC1
#define ADC_CHANNEL LL_ADC_CHANNEL_1
#define ADC_GPIO_PORT GPIOA
#define ADC_GPIO_PIN LL_GPIO_PIN_1

/* Local Types and Typedefs -------------------------------------------------*/

/* Global Variables ---------------------------------------------------------*/
/** Test message sent over UART DMA for demonstration purposes. */
static char testMessage[] = "Hello\r\n";

/** Aggregate Fault Manager state retained for debugger observation. */
static volatile bool g_testFaultActive = false;

/* Private Function Prototypes ----------------------------------------------*/
static void TestTask(void *pvParameters);
static void FltManTask(void *pvParameters);
static void GPIO_Config(void);
static void ADC_Config(void);
static void Activate_ADC(void);
static void ADC_Calibrate_SE_LL(ADC_TypeDef *adc);
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

    // Create the dedicated Fault Manager task before symptom producers.
    xTaskCreate(FltManTask, "FltManTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);

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

    GPIO_Config();
    ADC_Config();
    Activate_ADC();
    for (;;)
    {
        bool loggerPoolExhausted = false;

        Logger_Entry_T *entry = logger_alloc_entry(loggerCtx); // Allocate a log entry
        if (entry)
        {
            strcpy((char *)entry->msg, testMessage); // Copy the test message into the log entry
            entry->length = sizeof(testMessage) - 1; // Set the length of the message
            logger_commit_entry(loggerCtx, entry);   // Commit the log entry for transmission
        }
        else
        {
            loggerPoolExhausted = true;
            logger_trigger_highprio(loggerCtx, CFG_LOGGER_ALLOC_FAILED, xTaskGetTickCount());
        }

        FltMan_SetSymptom(&g_flt_man, SYMPTOM_SW_BUFFER_OVERFLOW, loggerPoolExhausted);

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(TEST_TASK_PERIOD_MS));
    }
}

/**
 * @brief Periodically evaluate the statically configured Fault Manager.
 *
 * This task is the sole owner of fault evaluation and derived fault-state
 * queries. Other tasks only publish symptom levels through the public API.
 *
 * @param[in] pvParameters Unused task parameter.
 */
static void FltManTask(void *pvParameters)
{
    (void)pvParameters;

    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        FltMan_Tick(&g_flt_man);
        g_testFaultActive = FltMan_IsAnyFaultActive(&g_flt_man);

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(FLT_MAN_TASK_PERIOD_MS));
    }
}

/* ─────────────────────────────────────────────── */
/* ADC CONFIGURATION                              */
/* ─────────────────────────────────────────────── */
static void ADC_Config(void)
{
    /* Configure NVIC to enable ADCx interruptions */
    NVIC_SetPriority(ADC1_2_IRQn, 0);
    NVIC_EnableIRQ(ADC1_2_IRQn);

    LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSOURCE_HCLK);
    LL_RCC_SetADCPrescaler(1);
    /* Peripheral clock enable */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ADC12);

    LL_ADC_CommonInitTypeDef adc_common_init = {0};
    LL_ADC_InitTypeDef adc_init = {0};
    LL_ADC_REG_InitTypeDef adc_reg_init = {0};

    /* Common init */
    LL_ADC_CommonStructInit(&adc_common_init);
    adc_common_init.Multimode = LL_ADC_MULTI_INDEPENDENT;
    LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADCx), &adc_common_init);

    /* ADC init */
    LL_ADC_StructInit(&adc_init);
    adc_init.Resolution = LL_ADC_RESOLUTION_12B;
    adc_init.LeftBitShift = LL_ADC_LEFT_BIT_SHIFT_NONE;
    adc_init.LowPowerMode = LL_ADC_LP_MODE_NONE;
    LL_ADC_Init(ADCx, &adc_init);

    /* Regular channel init */
    LL_ADC_REG_StructInit(&adc_reg_init);
    adc_reg_init.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
    adc_reg_init.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
    adc_reg_init.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    adc_reg_init.ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS;
    adc_reg_init.DataTransferMode = LL_ADC_REG_DR_TRANSFER;
    adc_reg_init.Overrun = LL_ADC_REG_OVR_DATA_OVERWRITTEN;
    LL_ADC_REG_Init(ADCx, &adc_reg_init);
    LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);
    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADCx), LL_ADC_PATH_INTERNAL_NONE);
    LL_ADC_DisableDeepPowerDown(ADCx);
    LL_ADC_SetGainCompensation(ADCx, 0);
    LL_ADC_SetChannelPreselection(ADCx, 1);

    /* Delay for ADC internal voltage regulator stabilization. */
    /* Compute number of CPU cycles to wait for, from delay in us. */
    /* Note: Variable divided by 2 to compensate partially */
    /* CPU processing cycles (depends on compilation optimization). */
    /* Note: If system core clock frequency is below 200kHz, wait time */
    /* is only a few CPU processing cycles. */
    uint32_t wait_loop_index;
    wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
    while (wait_loop_index != 0)
    {
        wait_loop_index--;
    }

    /** Configure Regular Channel
     */
    LL_ADC_REG_SetSequencerRanks(ADCx, LL_ADC_REG_RANK_1, ADC_CHANNEL);
    LL_ADC_SetChannelSamplingTime(ADCx, ADC_CHANNEL, LL_ADC_SAMPLINGTIME_1499CYCLES_5);
    LL_ADC_SetChannelSingleDiff(ADCx, ADC_CHANNEL, LL_ADC_SINGLE_ENDED);
}

static void Activate_ADC(void)
{
    if (LL_ADC_IsEnabled(ADCx) == 0)
    {

        /* 1. Enable analog power domain */
        LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_PWR);
        LL_PWR_EnableVddADC();

        while (!LL_PWR_IsEnabledVddADC())
            ;
        LL_ADC_Disable(ADCx);
        LL_mDelay(10);

        /* Enable ADC */
        LL_ADC_Enable(ADCx);

        while (LL_ADC_IsActiveFlag_ADRDY(ADCx) == 0)
        {
        }
        LL_mDelay(10);
    }
    LL_ADC_REG_StartConversion(ADCx);
}

/* ─────────────────────────────────────────────── */
/* GPIO CONFIGURATION (PA9 → analog)              */
/* ─────────────────────────────────────────────── */
static void GPIO_Config(void)
{
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOA);

    LL_GPIO_InitTypeDef gpio_init = {0};
    gpio_init.Pin = ADC_GPIO_PIN;
    gpio_init.Mode = LL_GPIO_MODE_ANALOG;
    gpio_init.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(ADC_GPIO_PORT, &gpio_init);

    LL_GPIO_DisablePinSecure(GPIOA, ADC_GPIO_PIN);
    LL_GPIO_DisablePinPrivilege(GPIOA, ADC_GPIO_PIN);
}

static void ADC_Calibrate_SE_LL(ADC_TypeDef *adc)
{
    const uint32_t N = 8;
    uint32_t sum = 0;

    // Force safe CFGR1 settings during cal
    MODIFY_REG(adc->CFGR1,
               ADC_CFGR1_DMNGT | ADC_CFGR1_EXTEN | ADC_CFGR1_CONT | ADC_CFGR1_RES,
               0);

    // Clear offsets & extra offset
    LL_ADC_SetOffsetLevel(adc, LL_ADC_OFFSET_1, 0);
    LL_ADC_SetOffsetLevel(adc, LL_ADC_OFFSET_2, 0);
    LL_ADC_SetOffsetLevel(adc, LL_ADC_OFFSET_3, 0);
    LL_ADC_SetOffsetLevel(adc, LL_ADC_OFFSET_4, 0);
    LL_ADC_DisableCalibrationOffset(adc); // CALADDOS = 0

    // Longish sample time = stable reads
    // (channel doesn’t matter in cal path, but LL asks for one)
    LL_ADC_SetChannelSamplingTime(adc, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_1499CYCLES_5);

    // Enable ADC & wait ready (N6 expects ADEN=1 for SW-cal routine)
    LL_ADC_Enable(adc);
    while (!LL_ADC_IsActiveFlag_ADRDY(adc))
    {
    }

    // Select single-ended cal path and "enter" cal mode
    CLEAR_BIT(adc->CR, ADC_CR_ADCALDIF);
    SET_BIT(adc->CR, ADC_CR_ADCAL); // ADCAL=1: conversions now measure offset path

    // Take N samples, average
    for (uint32_t i = 0; i < N; ++i)
    {
        LL_ADC_REG_StartConversion(adc);
        while (LL_ADC_REG_IsConversionOngoing(adc))
        {
        }
        sum += LL_ADC_REG_ReadConversionData32(adc);
    }
    uint32_t avg = sum / N;

    // If avg <= 0, enable extra offset and repeat once (per RM)
    if ((int32_t)avg <= 0)
    {
        LL_ADC_EnableCalibrationOffset(adc); // CALADDOS=1
        sum = 0;
        for (uint32_t i = 0; i < N; ++i)
        {
            LL_ADC_REG_StartConversion(adc);
            while (LL_ADC_REG_IsConversionOngoing(adc))
            {
            }
            sum += LL_ADC_REG_ReadConversionData32(adc);
        }
        avg = sum / N;
        if ((int32_t)avg <= 0)
            return; // should not happen
    }

    // Program CALFACT_S while ADC enabled & idle
    LL_ADC_SetCalibrationFactor(adc, LL_ADC_SINGLE_ENDED, (avg & 0x1FF));

    // Leave calibration mode
    LL_ADC_StopCalibration(adc); // clears ADCAL
}
