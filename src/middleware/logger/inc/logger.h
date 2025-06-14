/**
 * @file logger.h
 * @brief Public interface for the real-time logger component
 *
 * The logger provides time-stamped debug output with separate pools for
 * high-priority and normal messages. It is designed to be ISR-safe and
 * to operate without dynamic memory allocation.
 */

#ifndef LOGGER_H
#define LOGGER_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "logger_cfg.h"
#include "cfg_logger.h"
#include "FreeRTOS.h"
#include "task.h"
/* Macros and Defines -------------------------------------------------------*/
/**
 * @brief Helper macro to statically initialize a ::Logger_Context_T object.
 *
 * All counters and buffers are cleared so the logger starts in a
 * known state.
 */
#define LOGGER_CONTEXT_INIT                                                            \
    {                                                                                 \
        .high_prio_mask = 0,                                                          \
        .high_prio_registry = {0},                                                    \
        .regular_log_pool = {0},                                                      \
        .regular_log_queue = {0},                                                     \
        .log_head = 0,                                                                \
        .log_tail = 0,                                                                \
        .debug_buffer = {0},                                                          \
        .debug_idx = 0                                                                \
    }

/**
 * @brief Convenience macro for defining static high-priority log entries.
 *
 * This helper should only be used for high-priority messages that are
 * statically allocated. Regular log entries must be obtained via
 * ::logger_alloc_entry so they are taken from the internal memory pool.
 */
#define LOGGER_DEFINE_HIGHPRIO_ENTRY(name, literal) \
    static Logger_Entry_T name = {                  \
        .prefix = {0},                              \
        .msg = literal,                             \
        .length = sizeof(literal) - 1,              \
        .base_length = sizeof(literal) - 1,         \
        .in_use = false,                            \
        .is_formatted = false                      \
    }

/**
 * @brief Convenience macro for defining static high-priority log entries.
 *
 * The created ::Logger_Entry_T is stored in read-only memory and can be
 * triggered directly from interrupts using ::logger_trigger_highprio.
 * Normal log entries should instead be allocated from the logger pool.
 */


/**
 * @brief Log entry structure
 */
typedef struct
{
    char prefix[LOGGER_PREFIX_SIZE];           /**< Formatted prefix */
    uint8_t msg[LOGGER_LOG_ENTRY_BUFFER_SIZE]; /**< Log message text */
    uint32_t length;                           /**< Length of the message */
    uint32_t base_length;                      /**< Length of the template message */
    uint32_t timestamp;                        /**< Log timestamp */
    bool in_use;                               /**< Allocation flag */
    bool is_formatted;                         /**< Timestamp already prepended */
} Logger_Entry_T;

typedef struct Logger_Context_Tag
{
    volatile uint32_t high_prio_mask;                                 /**< Bitmask for high-priority logs */
    Logger_Entry_T *high_prio_registry[LOGGER_HIGH_PRIO_LOGS_NUMBER]; /**< Registry for high-priority log entries */
    Logger_Entry_T regular_log_pool[LOGGER_LOG_QUEUE_SIZE];           /**< Pool for normal-priority log entries */
    Logger_Entry_T *regular_log_queue[LOGGER_LOG_QUEUE_SIZE];         /**< Queue for normal-priority log entries */
    volatile uint8_t log_head;                                        /**< Head index of the normal log queue */
    volatile uint8_t log_tail;                                        /**< Tail index of the normal log queue */
    TaskHandle_t logger_task_handle;                                  /**< Handle for the logger task */
    volatile uint32_t debug_buffer[LOGGER_DEBUG_BUFFER_SIZE];         /**< Buffer for raw debug values */
    volatile uint16_t debug_idx;                                      /**< Write index for debug buffer */
} Logger_Context_T;

/**
 * @brief Allocates a log entry from the static pool
 * @return Pointer to Logger_Entry_T if available, NULL otherwise
 */
Logger_Entry_T *logger_alloc_entry(Logger_Context_T *ctx);

/**
 * @brief Commits a normal-priority log entry for asynchronous transmission
 * @param entry Pointer to the log entry
 */
void logger_commit_entry(Logger_Context_T *ctx, Logger_Entry_T *entry);

/**
 * @brief Registers a preallocated high-priority log entry
 * @param idx High-priority slot index (0–31)
 * @param entry Pointer to statically defined Logger_Entry_T
 */
void logger_trigger_highprio(Logger_Context_T *ctx, uint8_t idx, uint32_t timestamp);

/**
 * @brief Logger transmission scheduler (called by logger task)
 *
 * Attempts to transmit the next pending log entry. If the underlying
 * UartDma_Transmit() call fails, the entry remains queued/registered and
 * the logger task is notified to retry. Retries continue until the
 * transmission succeeds.
 */
void logger_tx_scheduler(Logger_Context_T *ctx);

/**
 * @brief RTOS task function that handles logger transmission
 *
 * The task waits for notifications from producers and forwards log
 * entries to the underlying UART DMA driver. It should be created with
 * a small stack size and runs at a low priority.
 *
 * @param arg Unused pointer to the logger context
 */
void logger_tx_task(void *arg);

/**
 * @brief Store a 32-bit debug value for later inspection
 *
 * Values recorded with this API are stored in a circular buffer inside
 * the ::Logger_Context_T structure. They can be examined using a debugger
 * when tracing complex issues.
 *
 * @param value Value to record in the debug buffer
 */
void logger_debug_push(Logger_Context_T *ctx, uint32_t value);

void logger_register_highprio(Logger_Context_T *ctx, uint8_t idx, Logger_Entry_T *entry);

#endif /* LOGGER_H */
