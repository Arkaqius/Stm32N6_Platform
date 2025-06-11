/**
 * @file SysM.h
 * @brief Brief description of the file purpose
 *
 * Detailed description of the module or file functionality.
 */

#ifndef SYSM_H
#define SYSM_H

/* Includes -----------------------------------------------------------------*/
#include "logger.h"
/* Macros and Defines -------------------------------------------------------*/

/* Typedefs -----------------------------------------------------------------*/

/* Exported Variables -------------------------------------------------------*/

/* Exported Interfaces ------------------------------------------------------*/
/**
 * @brief Obtain the global logger context used by the application.
 *
 * @return Pointer to the statically allocated Logger_Context_T instance.
 */
Logger_Context_T *Cfg_Logger_GetContext(void);

/**
 * @brief Initialize the application logger and register static messages.
 */
void Cfg_Logger_Init(void);

#endif /* SYSM_H */
