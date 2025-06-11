/**
 * @file UartDma.h
 * @brief UART transmission driver using DMA
 *
 * The driver offers a simple zero-copy API for sending data over a UART
 * peripheral.  A lightweight non-blocking lock ensures that transfers are
 * serialized without blocking the caller.  It is intended for use in
 * real-time systems where minimal latency is required.
 */

#ifndef UART_DMA_H
#define UART_DMA_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "stm32n6xx_ll_usart.h"
#include "stm32n6xx_ll_gpio.h"
#include "stm32n6xx_ll_dma.h"

/* Macros and Defines -------------------------------------------------------*/

/* Typedefs -----------------------------------------------------------------*/
/**
 * @brief Internal UART DMA driver state
 */
typedef struct
{
    uint8_t is_busy; /**< Flag indicating active DMA transfer */
} UartDma_Handler_T;

/* Exported Variables -------------------------------------------------------*/

/* Exported Interfaces ------------------------------------------------------*/
/**
 * @brief Initialize the UART DMA driver and its peripherals.
 *
 * This function configures GPIO pins, the USART peripheral and the DMA
 * channel required for transmission.  It also creates any internal tasks
 * used by the driver.
 *
 * @return true on success, false otherwise.
 */
bool UartDma_Init(void);

/**
 * @brief Schedule a buffer for transmission via DMA.
 *
 * The function returns immediately after queuing the transfer. If the DMA
 * channel is busy the call fails and the data should be retried later.
 *
 * @param[in] data Pointer to the buffer to transmit.
 * @param[in] size Number of bytes contained in the buffer.
 *
 * @return true if the buffer was accepted for transmission.
 */
bool UartDma_Transmit(const uint8_t *data, uint16_t size);

#endif /* UART_DMA_H */
