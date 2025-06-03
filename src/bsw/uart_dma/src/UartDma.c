/**
 * @file UartDma.c
 * @brief Implementation of UART transmission using DMA with non-blocking lock.
 * @ingroup UartDMA
 * @{
 *
 * This module provides UART transmission using DMA with a non-blocking
 * LDREX/STREX-based lock. Designed for hard real-time systems with zero-copy,
 * deterministic transmission scheduling.
 */

/* Includes ------------------------------------------------------------------*/
#include "UartDma.h"
#include "stm32n6xx_ll_usart.h"
#include "stm32n6xx_ll_dma.h"
#include "stm32n6xx_ll_gpio.h"
#include "stm32n6xx_ll_bus.h"
#include "stm32n6xx_ll_rcc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_gcc.h"

/* Defines -------------------------------------------------------------------*/
#define USED_UART_INSTANCE USART1 /**< Define the UART instance to be used */

/* Global Variables ----------------------------------------------------------*/
/**
 * @brief UART DMA handler instance.
 */
static UartDma_Handler_T g_uartDmaHandler = {0};

/* Private Function Prototypes -----------------------------------------------*/
static void UartDma_MainTask(void *pvParameters);
static bool UartDma_InitUsart(void);
static bool UartDma_InitGpio(void);
static bool UartDma_InitDma(void);
static bool UartDma_ErrorHandler(void);
static void UartDma_TasksInit(void);

/**
 * @brief Initialize the UART DMA module.
 *
 * Configures GPIO, USART, and DMA peripherals, and starts associated FreeRTOS tasks.
 *
 * @return true if initialization was successful.
 */
bool UartDma_Init(void)
{
    UartDma_InitGpio();
    UartDma_InitUsart();
    UartDma_InitDma();
    UartDma_TasksInit();
    return true;
}

/**
 * @brief Attempt to transmit data using DMA in a non-blocking fashion.
 *
 * This function uses an exclusive LDREX/STREX based lock to avoid contention
 * and ensures zero-copy, low-latency transmission scheduling.
 *
 * @param[in] data Pointer to the data buffer to transmit.
 * @param[in] size Number of bytes to transmit.
 * @return true if transmission was scheduled successfully, false otherwise.
 */
bool UartDma_Transmit(const uint8_t *data, uint16_t size)
{
    if (data == NULL || size == 0)
    {
        return false;
    }

    uint8_t current;
    uint32_t result;

    current = __LDREXB((uint8_t *)&g_uartDmaHandler.is_busy);
    if (current != 0)
    {
        __CLREX();
        return false;
    }

    result = __STREXB(1, (uint8_t *)&g_uartDmaHandler.is_busy);
    if (result != 0)
    {
        __CLREX();
        return false;
    }

    SCB_CleanDCache_by_Addr((uint32_t *)data, size);

    LL_DMA_ConfigAddresses(GPDMA1, LL_DMA_CHANNEL_0,
                           (uint32_t)data, LL_USART_DMA_GetRegAddr(USART1, LL_USART_DMA_REG_DATA_TRANSMIT));
    LL_DMA_SetBlkDataLength(GPDMA1, LL_DMA_CHANNEL_0, size);
    LL_USART_EnableDMAReq_TX(USART1);
    LL_DMA_EnableChannel(GPDMA1, LL_DMA_CHANNEL_0);

    return true;
}

static bool UartDma_InitUsart(void)
{
    LL_USART_InitTypeDef usart_h;

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
    LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK2);

    usart_h.PrescalerValue = LL_USART_PRESCALER_DIV1;
    usart_h.BaudRate = 115200;
    usart_h.DataWidth = LL_USART_DATAWIDTH_8B;
    usart_h.StopBits = LL_USART_STOPBITS_1;
    usart_h.Parity = LL_USART_PARITY_NONE;
    usart_h.TransferDirection = LL_USART_DIRECTION_TX;
    usart_h.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    usart_h.OverSampling = LL_USART_OVERSAMPLING_16;

    LL_USART_Init(USART1, &usart_h);
    LL_USART_SetTXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_8);
    LL_USART_SetRXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_8);
    LL_USART_DisableFIFO(USART1);
    LL_USART_ConfigAsyncMode(USART1);
    LL_USART_Enable(USART1);

    return true;
}

static bool UartDma_InitGpio(void)
{
    LL_GPIO_InitTypeDef gpio_h;

    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOE);

    gpio_h.Pin = LL_GPIO_PIN_5 | LL_GPIO_PIN_6;
    gpio_h.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_h.Pull = LL_GPIO_PULL_NO;
    gpio_h.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    gpio_h.Alternate = LL_GPIO_AF_7;
    LL_GPIO_Init(GPIOE, &gpio_h);

    return true;
}

static bool UartDma_InitDma(void)
{
    LL_DMA_InitTypeDef dma_h;

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPDMA1);
    LL_DMA_StructInit(&dma_h);

    dma_h.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    dma_h.DataAlignment = LL_DMA_DATA_ALIGN_SIGNEXTPADD;
    dma_h.SrcDataWidth = LL_DMA_DEST_DATAWIDTH_HALFWORD;
    dma_h.DestDataWidth = LL_DMA_DEST_DATAWIDTH_BYTE;
    dma_h.SrcIncMode = LL_DMA_SRC_INCREMENT;
    dma_h.DestIncMode = LL_DMA_DEST_FIXED;
    dma_h.Priority = LL_DMA_LOW_PRIORITY_HIGH_WEIGHT;
    dma_h.TriggerMode = LL_DMA_TRIGM_BLK_TRANSFER;
    dma_h.Request = LL_GPDMA1_REQUEST_USART1_TX;

    LL_DMA_Init(GPDMA1, LL_DMA_CHANNEL_0, &dma_h);

    NVIC_SetPriority(GPDMA1_Channel0_IRQn, 0);
    NVIC_EnableIRQ(GPDMA1_Channel0_IRQn);
    LL_DMA_EnableIT_TC(GPDMA1, LL_DMA_CHANNEL_0);

    return true;
}

static bool UartDma_ErrorHandler(void)
{
    return false;
}

static void UartDma_TasksInit(void)
{
    xTaskCreate(UartDma_MainTask, "UartDmaMainTask", 1000, NULL, 1, NULL);
}

static void UartDma_MainTask(void *pvParameters)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief DMA interrupt handler for UART DMA channel.
 *
 * Clears transfer complete or error flags and releases the DMA lock.
 *
 * @bug Add it to ISR manager
 */
void GPDMA1_Channel0_IRQHandler(void)
{
    if (LL_DMA_IsActiveFlag_TC(GPDMA1, LL_DMA_CHANNEL_0) && LL_DMA_IsEnabledIT_TC(GPDMA1, LL_DMA_CHANNEL_0))
    {
        LL_DMA_ClearFlag_TC(GPDMA1, LL_DMA_CHANNEL_0);
        g_uartDmaHandler.is_busy = 0;
        __DMB();
    }
    else if ((LL_DMA_IsActiveFlag_USE(GPDMA1, LL_DMA_CHANNEL_0) && LL_DMA_IsEnabledIT_USE(GPDMA1, LL_DMA_CHANNEL_0)) ||
             (LL_DMA_IsActiveFlag_ULE(GPDMA1, LL_DMA_CHANNEL_0) && LL_DMA_IsEnabledIT_ULE(GPDMA1, LL_DMA_CHANNEL_0)) ||
             (LL_DMA_IsActiveFlag_DTE(GPDMA1, LL_DMA_CHANNEL_0) && LL_DMA_IsEnabledIT_DTE(GPDMA1, LL_DMA_CHANNEL_0)))
    {
        LL_DMA_ClearFlag_USE(GPDMA1, LL_DMA_CHANNEL_0);
        LL_DMA_ClearFlag_ULE(GPDMA1, LL_DMA_CHANNEL_0);
        LL_DMA_ClearFlag_DTE(GPDMA1, LL_DMA_CHANNEL_0);
        UartDma_ErrorHandler();
    }
}

/** @} */ // end of UartDMA group