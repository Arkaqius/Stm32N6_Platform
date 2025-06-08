/**
 * @file cfg_logger.h
 * @brief Brief description of the file purpose
 *
 * Detailed description of the module or file functionality.
 */

#ifndef CFGCONST_LOGGER_H
#define CFGCONST_LOGGER_H

/* Includes -----------------------------------------------------------------*/
/* Macros and Defines -------------------------------------------------------*/
#define CFG_LOGGER_BUFFER_SIZE_DEFAULT (1024U)  /**< Default buffer size for the logger */
#define CFG_LOGGER_HIGH_PRIO_LOGS_NUMBER (10U)  /**< Default number of high priority logs */
#define CFG_LOGGER_LOG_ENTRY_BUFFER_SIZE (256U) /**< Default size of each log entry buffer */
#define CFG_LOGGER_LOG_QUEUE_SIZE (32U)         /**< Default size of the log queue */
#define CFG_LOGGER_DEBUG_BUFFER_SIZE (128U)      /**< Default size of the debug value buffer */

/** Index of the "queue full" high priority message. */
#define CFG_LOGGER_HP_QUEUE_FULL_IDX (0U)
/** Text of the "queue full" high priority message. */
#define CFG_LOGGER_HP_QUEUE_FULL_MSG "Queue FULL\r\n"

/* Typedefs -----------------------------------------------------------------*/

#endif /* CFGCONST_LOGGER_H */
