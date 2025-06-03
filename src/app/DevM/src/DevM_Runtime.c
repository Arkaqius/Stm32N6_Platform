/**
 * @file DevM_Runtime.c
 * @brief Device Manager Runtime Implementation
 *
 * This file contains the implementation of the runtime state machine
 * and related functions for the Device Manager module.
 */

/* Includes -----------------------------------------------------------------*/
#include "DevM.h"
#include "DevM_PostOS.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "DevM_PreOS.h"
#include "DevM_Runtime.h"
/* Defines ------------------------------------------------------------------*/

/* Local Types and Typedefs -------------------------------------------------*/

/* Global Variables ---------------------------------------------------------*/
/**< Current state of the Device Manager state machine. */
static DevM_StateType currentState = DEVM_STATE_FAULT;

/* Private Function Prototypes ----------------------------------------------*/
static DevM_ReturnType DevM_StateRun(void);
static DevM_ReturnType DevM_StateFault(void);
static DevM_ReturnType DevM_StateSoftRestart(void);
/* Public Functions Implementation ------------------------------------------*/
/**
 * @brief Run one iteration of the Device Manager state machine.
 */
void DevM_RunStateMachine(void)
{
    DevM_ReturnType ret = DEVM_OK;

    switch (currentState)
    {

    case DEVM_STATE_INIT_POST_OS:
        ret = DevM_StateInitPostOS();
        currentState = (ret == DEVM_OK) ? DEVM_STATE_RUN : DEVM_STATE_FAULT;
        break;

    case DEVM_STATE_RUN:
        DevM_StateRun();
        break;

    case DEVM_STATE_FAULT:
        DevM_StateFault();
        break;

    case DEVM_STATE_SOFT_RESTART:
        DevM_StateSoftRestart();
        break;

    case DEVM_STATE_INIT_PRE_OS:
    case DEVM_STATE_INIT_OS:
    default:
        currentState = DEVM_STATE_FAULT;
        break;
    }
}
/* Private Functions Implementation -----------------------------------------*/

/**
 * @brief Fault handling routine.
 * @return Does not return.
 */
static __attribute__((noreturn)) DevM_ReturnType DevM_StateFault(void)
{
    while (1)
    {
        /* We just hang here forever */
    }
}

/**
 * @brief Perform a software-triggered system reset.
 * @return Never reached.
 */
static DevM_ReturnType DevM_StateSoftRestart(void)
{
    /* TODO Make Soft reset */
    return DEVM_OK;
}

/**
 * @brief Normal operating state handling.
 */
static DevM_ReturnType DevM_StateRun(void) { return DEVM_OK; }

/**
 * @brief Execute the full state machine until run or fault state.
 */
void DevM_MainFunction(void *params)
{
    void *pvParameters = params;

    uint32_t receivedEvent;

    for (;;)
    {
        if (xQueueReceive(devmEventQueue, &receivedEvent, portMAX_DELAY) == pdPASS)
        {
            /* Process the event through the state machine */
            DevM_RunStateMachine();
        }
    }
}
