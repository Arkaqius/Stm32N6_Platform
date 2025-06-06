/**
 * @file logger.c
 * @brief Real-time, dual-priority, ISR-safe, zero-copy logger implementation
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "logger.h"
#include "UartDma.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_gcc.h"

/** Remove and return the next regular log entry from the queue. */
static inline Logger_Entry_T *dequeue_normal_log(Logger_Context_T *ctx);
/** Queue a normal-priority log entry for transmission. */
static inline void enqueue_normal_log(Logger_Context_T *ctx, Logger_Entry_T *entry);
/** Peek the next log entry in the queue without removing it. */
static inline Logger_Entry_T *peek_normal_log(Logger_Context_T *ctx);
/** Format a log entry by prepending a timestamp. */
static bool format_log_entry(Logger_Entry_T *entry);
/*** Logger API ***/
/**
 * @brief Allocates a log entry from the static pool.
 * @return Pointer to a Logger_Entry_T if available, NULL if pool is full.
 */
Logger_Entry_T *logger_alloc_entry(Logger_Context_T *ctx)
{
    for (uint8_t i = 0; i < LOGGER_LOG_QUEUE_SIZE; i++)
    {
        bool expected = false;
        /*
         * Use an atomic compare-and-exchange to safely claim the entry.
         * Without this, concurrent callers could observe the entry as
         * available and allocate it multiple times, leading to lost logs.
         */
        if (__atomic_compare_exchange_n(&ctx->regular_log_pool[i].in_use,
                                        &expected,
                                        true,
                                        false,
                                        __ATOMIC_ACQUIRE,
                                        __ATOMIC_RELAXED))
        {

            ctx->regular_log_pool[i].is_formatted = false;
            return &(ctx->regular_log_pool[i]);
        }
    }
    return NULL;
}

/**
 * @brief Commits a log entry for asynchronous transmission.
 * @param entry Pointer to a filled Logger_Entry_T.
 */
void logger_commit_entry(Logger_Context_T *ctx, Logger_Entry_T *entry)
{
    entry->timestamp = xTaskGetTickCount();
    logger_debug_push(ctx, (uint32_t)entry->timestamp); // Push the current tick count to the debug buffer
    logger_debug_push(ctx, (uint32_t)entry);            // Push the current tick count to the debug buffer
    entry->is_formatted = false;
    enqueue_normal_log(ctx, entry);
    xTaskNotifyGive(ctx->logger_task_handle);
}

/**
 * @brief Triggers a high-priority preallocated log entry from ISR.
 * @param idx Index of the registered high-priority log.
 * @param timestamp Timestamp to assign to the log entry.
 */
void logger_trigger_highprio(Logger_Context_T *ctx, uint8_t idx, uint32_t timestamp)
{
    if (idx >= LOGGER_HIGH_PRIO_LOGS_NUMBER)
        return;
    Logger_Entry_T *entry = ctx->high_prio_registry[idx];
    if (!entry)
        return;
    if (entry->length > entry->base_length)
    {
        uint8_t diff = entry->length - entry->base_length;
        memmove(entry->msg, entry->msg + diff, entry->base_length);
        memset(entry->msg + entry->base_length, 0, diff);
        entry->length = entry->base_length;
    }
    entry->timestamp = timestamp;
    entry->in_use = true;
    entry->is_formatted = false;
    __atomic_or_fetch(&(ctx->high_prio_mask), (1u << idx), __ATOMIC_RELAXED);
    xTaskNotifyGive(ctx->logger_task_handle);
}

/**
 * @brief Scheduler responsible for selecting and transmitting log entries.
 */
void logger_tx_scheduler(Logger_Context_T *ctx)
{
    Logger_Entry_T *entry = NULL;

    // 1. High-priority logs
    uint32_t pending = ctx->high_prio_mask;
    if (pending)
    {
        uint32_t idx = 31 - __builtin_clz(pending);
        entry = ctx->high_prio_registry[idx];
        if (entry && entry->in_use)
        {
            bool isSent = false;
            bool isReady = entry->is_formatted ? true : format_log_entry(entry);
            if (isReady)
            {
                isSent = UartDma_Transmit((uint8_t *)&entry->msg[0], entry->length);
            }

            if (isSent)
            {
                entry->in_use = false;
                entry->is_formatted = false;
                __atomic_and_fetch(&(ctx->high_prio_mask), ~(1u << idx), __ATOMIC_RELAXED);
            }
            else
            {
                xTaskNotifyGive(ctx->logger_task_handle);
            }
            return;
        }
    }

    // 2. Normal log queue
    entry = peek_normal_log(ctx);
    if (entry)
    {
        bool isSent = false;
        bool isReady = entry->is_formatted ? true : format_log_entry(entry);
        if (isReady)
        {
            isSent = UartDma_Transmit((uint8_t *)&entry->msg[0], entry->length);
        }

        if (isSent)
        {
            dequeue_normal_log(ctx);
            entry->in_use = false;
            entry->is_formatted = false;
        }
        else
        {
            xTaskNotifyGive(ctx->logger_task_handle);
        }
    }
}

/**
 * @brief Registers a preallocated high-priority log entry.
 * @param idx Index to assign the log entry (0-31).
 * @param entry Pointer to the Logger_Entry_T to register.
 */
void logger_register_highprio(Logger_Context_T *ctx, uint8_t idx, Logger_Entry_T *entry)
{
    if (idx < LOGGER_HIGH_PRIO_LOGS_NUMBER)
    {
        entry->base_length = entry->length;
        ctx->high_prio_registry[idx] = entry;
        /* Handle internal state */
        ctx->high_prio_registry[idx]->in_use = false;       // Initially not in use
        ctx->high_prio_registry[idx]->is_formatted = false; // Not formatted yet
    }
}

/**
 * @brief Logger transmission task to be run in a dedicated RTOS task.
 * @param arg Unused task argument.
 */
void logger_tx_task(void *arg)
{
    Logger_Context_T *ctx = (Logger_Context_T *)arg;

    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        logger_tx_scheduler(ctx);
    }
}

void logger_debug_push(Logger_Context_T *ctx, uint32_t value)
{
    uint16_t idx = __atomic_fetch_add(&ctx->debug_idx, 1, __ATOMIC_RELAXED);
    ctx->debug_buffer[idx % LOGGER_DEBUG_BUFFER_SIZE] = value;
}

/*** Helper Functions ***/
/**
 * @brief Place a log entry into the regular log queue.
 */
static inline void enqueue_normal_log(Logger_Context_T *ctx, Logger_Entry_T *entry)
{
    uint8_t next = (ctx->log_head + 1) % LOGGER_LOG_QUEUE_SIZE;
    if (next != ctx->log_tail)
    { // Check if queue is not full
        ctx->regular_log_queue[ctx->log_head] = entry;
        ctx->log_head = next;
    }
    else
    {
        entry->in_use = false; // Drop log if queue full
        entry->is_formatted = false;
    }
}

/**
 * @brief Remove and return the oldest log entry from the queue.
 */
static inline Logger_Entry_T *dequeue_normal_log(Logger_Context_T *ctx)
{
    if (ctx->log_tail == ctx->log_head) // Queue is empty
    {
        return NULL;
    }
    Logger_Entry_T *e = ctx->regular_log_queue[ctx->log_tail];
    ctx->log_tail = (ctx->log_tail + 1) % LOGGER_LOG_QUEUE_SIZE;
    return e;
}

/**
 * @brief Get a pointer to the next log entry without dequeuing it.
 */
static inline Logger_Entry_T *peek_normal_log(Logger_Context_T *ctx)
{
    if (ctx->log_tail == ctx->log_head)
    {
        return NULL;
    }
    return ctx->regular_log_queue[ctx->log_tail];
}

/**
 * @brief Prepends a timestamp to a log entry's message buffer in-place.
 * @param entry Pointer to the Logger_Entry_T to modify.
 * @return true if formatting succeeded, false if buffer would overflow.
 */
static bool format_log_entry(Logger_Entry_T *entry)
{
    uint32_t ts = entry->timestamp;
    char ts_buf[12]; // e.g., "[123456] "
    uint8_t ts_len = 0;

    ts_buf[ts_len++] = '[';
    uint32_t div = 1000000000;
    bool started = false;
    while (div > 0)
    {
        uint32_t digit = ts / div;
        if (digit || started || div == 1)
        {
            ts_buf[ts_len++] = '0' + digit;
            started = true;
        }
        ts = ts % div;
        div /= 10;
    }
    ts_buf[ts_len++] = ']';
    ts_buf[ts_len++] = ' ';

    if ((entry->length + ts_len) >= LOGGER_LOG_ENTRY_BUFFER_SIZE)
        return false;

    memmove(&entry->msg[ts_len], &entry->msg[0], entry->length);
    memcpy(&entry->msg[0], ts_buf, ts_len);
    entry->length += ts_len;
    entry->is_formatted = true;

    return true;
}