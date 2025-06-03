/**
 * @file UartDma.h
 * @brief Brief description of the UART DMA module
 *
 * Detailed description of the UART DMA module functionality.
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
/**
 * @brief Example constant used within the UART DMA module.
 */
#define EXAMPLE_MODULE_CONSTANT (100U)

/* Typedefs -----------------------------------------------------------------*/
/**
 * @brief Example structure description
 */
typedef struct
{
    /* Runtime data */
    uint8_t is_busy;
} UartDma_Handler_T;

/* Exported Variables -------------------------------------------------------*/
/** Example global variable used by the module. */
extern uint32_t g_exampleGlobalVariable;

/* Exported Interfaces ------------------------------------------------------*/
/**
 * @brief Brief description of example interface function
 *
 * Detailed description of the function behavior.
 *
 * @param[in] param1 Description of first parameter
 * @param[out] result Description of output parameter
 *
 * @return Status code
 */
bool UartDma_Init(void);
bool UartDma_Transmit(const uint8_t *data, uint16_t size);

#endif /* UART_DMA_H */
