/**
 * @file SysM.c
 * @brief Brief description of source file implementation
 *
 * Detailed description about what this file implements or handles.
 */

/* Includes -----------------------------------------------------------------*/
#include "logger.h"
#include <stdbool.h>
#include "cfg_logger.h"
/* Defines ------------------------------------------------------------------*/

/* Local Types and Typedefs -------------------------------------------------*/

/* Global Variables ---------------------------------------------------------*/
/**
 * @brief Statically allocated application logger context.
 */
static Logger_Context_T logger_context = LOGGER_CONTEXT_INIT;

LOGGER_DEFINE_HIGHPRIO_ENTRY(hp_queue_full, CFG_LOGGER_HP_QUEUE_FULL_MSG);

/* Private Function Prototypes ----------------------------------------------*/

/* Public Functions Implementation ------------------------------------------*/
/**
 * @brief Get pointer to the logger context.
 *
 * @return Pointer to Logger_Context_T structure.
 */
Logger_Context_T *Cfg_Logger_GetContext(void)
{
    return &logger_context;
}

void Cfg_Logger_Init(void)
{
    logger_register_highprio(&logger_context,
                             CFG_LOGGER_HP_QUEUE_FULL_IDX,
                             &hp_queue_full);
}
/* Private Functions Implementation -----------------------------------------*/
/**
 * @brief Brief description of private helper function
 *
 * Detailed explanation of what the helper function does.
 *
 * @param[in] value Description of input parameter
 */