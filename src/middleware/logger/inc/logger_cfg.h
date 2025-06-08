/**
 * @file logger_cfg.h
 * @brief Default configuration for the logger component
 */

#ifndef LOGGER_CFG_H
#define LOGGER_CFG_H


#ifndef LOGGER_HIGH_PRIO_LOGS_NUMBER
#define LOGGER_HIGH_PRIO_LOGS_NUMBER (10U) /**< Default number of high priority logs */
#endif

#ifndef LOGGER_LOG_ENTRY_BUFFER_SIZE
#define LOGGER_LOG_ENTRY_BUFFER_SIZE (256U) /**< Default size of each log entry buffer */
#endif

#ifndef LOGGER_LOG_QUEUE_SIZE
#define LOGGER_LOG_QUEUE_SIZE (32U) /**< Default size of the log queue */
#endif

#ifndef LOGGER_DEBUG_BUFFER_SIZE
#define LOGGER_DEBUG_BUFFER_SIZE (1024U) /**< Default size of the debug value buffer */
#endif

#endif // LOGGER_CFG_H
