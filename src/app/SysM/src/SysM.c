/**
 * @file Cfg_logger.c
 * @brief Brief description of source file implementation
 *
 * Detailed description about what this file implements or handles.
 */

/* Includes -----------------------------------------------------------------*/
#include "logger.h"
/* Defines ------------------------------------------------------------------*/

/* Local Types and Typedefs -------------------------------------------------*/

/* Global Variables ---------------------------------------------------------*/
static Logger_Context_T logger_context = LOGGER_CONTEXT_INIT;

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
/* Private Functions Implementation -----------------------------------------*/
/**
 * @brief Brief description of private helper function
 *
 * Detailed explanation of what the helper function does.
 *
 * @param[in] value Description of input parameter
 */