/**
 * @file      DevManager.c
 * @copyright [copy write holder]
 *
 * @brief Device Manager state machine and singleton implementation.
 */

/*******************************************************************************
 * Includes
 *******************************************************************************/
#include "stm32n657xx.h"
#include "DevM.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "core_cm55.h"
#include "cmsis_gcc.h"
/*******************************************************************************
 * Defines
 *******************************************************************************/

/*******************************************************************************
 * Local Types and Typedefs
 *******************************************************************************/
/**
 * @brief State machine states for Device Manager.
 */
typedef enum DevM_StateType
{
  DEVM_STATE_INIT_PRE_OS,  /**< Combined Pre-OS initialization state */
  DEVM_STATE_INIT_OS,      /**< RTOS initialization state */
  DEVM_STATE_INIT_POST_OS, /**< Combined Post-OS initialization state */
  DEVM_STATE_RUN,          /**< Scheduler run state */
  DEVM_STATE_FAULT,        /**< Fault handling */
  DEVM_STATE_SOFT_RESTART  /**< System reset */
} DevM_StateType;

/**
 * @brief Return codes for Device Manager operations.
 */
typedef enum
{
  DEVM_OK = 0, /**< Success */
  DEVM_ERROR   /**< Failure */
} DevM_ReturnType;

/**
 * @brief Singleton structure for Device Manager.
 */
struct DevManager_T
{
  bool initialized; /**< Initialization flag */
};

/*******************************************************************************
 * Global Variables
 *******************************************************************************/
static DevManager_T g_DevManager_instance;
static DevM_StateType currentState = DEVM_STATE_FAULT;
static TaskHandle_t devmTaskHandle = NULL;
/* Queue handle for state events */
static QueueHandle_t devmEventQueue = NULL;
/*******************************************************************************
 * Static Function Prototypes
 *******************************************************************************/
/* Pre OS */
static DevM_ReturnType DevM_StateInitPreOS(void);
static DevM_ReturnType DevM_InitInfra(void);
static DevM_ReturnType DevM_StateInitBswPreOS(void);
static DevM_ReturnType DevM_StateInitMiddlewarePreOS(void);
static DevM_ReturnType DevM_StateInitServicesPreOS(void);
static DevM_ReturnType DevM_StateInitOS(void);

/* Post OS */
static DevM_ReturnType DevM_StateInitPostOS(void);
static DevM_ReturnType DevM_StateInitBswPostOS(void);
static DevM_ReturnType DevM_StateInitMiddlewarePostOS(void);
static DevM_ReturnType DevM_StateInitServicesPostOS(void);
static DevM_ReturnType DevM_StateInitApp(void);

/* Runtime states*/
static DevM_ReturnType DevM_StateRun(void);
static DevM_ReturnType DevM_StateFault(void);
static DevM_ReturnType DevM_StateSoftRestart(void);

/* TODO Task */
static void DevM_MainFunction(void *params);
/*******************************************************************************
 * Functions
 *******************************************************************************/
void DevM_Startup(void)
{
  DevManager_init();     /* Init DevManager */
  DevM_StateInitPreOS(); /* Call PreOs initializations */
  DevM_StateInitOS();    /* Call Os initialization */
  vTaskStartScheduler(); /* Dispatch contex switching to TaskScheduler */
}

/**
 * @brief Initialize the singleton instance of the Device Manager.
 * @return true if initialized for the first time, false if already initialized.
 */
bool DevManager_init(void)
{
  if (!g_DevManager_instance.initialized)
  {
    g_DevManager_instance.initialized = true;
    return true;
  }
  return false;
}

/**
 * @brief Get the instance of the Device Manager if initialized.
 * @return Pointer to DevManager_T or NULL if not initialized.
 */
DevManager_T *DevManager_getInstance(void)
{
  return g_DevManager_instance.initialized ? &g_DevManager_instance : NULL;
}

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

/**
 * @brief Initialize all pre-OS components.
 * @return DEVM_OK if successful.
 */
static DevM_ReturnType DevM_StateInitPreOS(void)
{
  if (DevM_InitInfra() != DEVM_OK)
    return DEVM_ERROR;
  if (DevM_StateInitBswPreOS() != DEVM_OK)
    return DEVM_ERROR;
  if (DevM_StateInitMiddlewarePreOS() != DEVM_OK)
    return DEVM_ERROR;
  if (DevM_StateInitServicesPreOS() != DEVM_OK)
    return DEVM_ERROR;
  return DEVM_OK;
}

static DevM_ReturnType DevM_InitInfra(void)
{
  SCB_EnableICache();
  SCB_EnableDCache();

  /* Configure priority grouping */
  NVIC_SetPriorityGrouping(4U);

  /* Update SystemCoreClock variable */
  SystemCoreClockUpdate();

  /* Configure SysTick to generate interrupt for FreeRTOS tick */
  if (SysTick_Config(SystemCoreClock / configTICK_RATE_HZ) != 0)
  {
    // Configuration failed
    while (1)
      ;
  }

  /* Set SysTick Interrupt priority */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),
                                                     configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0));

  return DEVM_OK;
}

/**
 * @brief Initialize all post-OS components.
 * @return DEVM_OK if successful.
 */
static DevM_ReturnType DevM_StateInitPostOS(void)
{
  if (DevM_StateInitBswPostOS() != DEVM_OK)
    return DEVM_ERROR;
  if (DevM_StateInitMiddlewarePostOS() != DEVM_OK)
    return DEVM_ERROR;
  if (DevM_StateInitServicesPostOS() != DEVM_OK)
    return DEVM_ERROR;
  if (DevM_StateInitApp() != DEVM_OK)
    return DEVM_ERROR;
  return DEVM_OK;
}

/**
 * @brief Fault handling routine.
 * @return Does not return.
 */
static DevM_ReturnType DevM_StateFault(void)
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

static DevM_ReturnType DevM_StateInitBswPreOS(void) { return DEVM_OK; }
static DevM_ReturnType DevM_StateInitMiddlewarePreOS(void) { return DEVM_OK; }
static DevM_ReturnType DevM_StateInitServicesPreOS(void) { return DEVM_OK; }
static DevM_ReturnType DevM_StateInitBswPostOS(void) { return DEVM_OK; }
static DevM_ReturnType DevM_StateInitMiddlewarePostOS(void) { return DEVM_OK; }
static DevM_ReturnType DevM_StateInitServicesPostOS(void) { return DEVM_OK; }
static DevM_ReturnType DevM_StateInitApp(void) { return DEVM_OK; }
static DevM_ReturnType DevM_StateRun(void) { return DEVM_OK; }

/* Implementation of OS initialization function */
static DevM_ReturnType DevM_StateInitOS(void)
{
  /* Create queue for state events */
  devmEventQueue = xQueueCreate(10, sizeof(uint32_t));
  if (devmEventQueue == NULL)
  {
    return DEVM_ERROR;
  }

  /* Create Task for DevM State Run */
  BaseType_t taskCreated = xTaskCreate(
      DevM_MainFunction,
      "DevM_Task",
      configMINIMAL_STACK_SIZE,
      NULL,
      tskIDLE_PRIORITY + 1,
      &devmTaskHandle);

  return (taskCreated == pdPASS) ? DEVM_OK : DEVM_ERROR;
}

/**
 * @brief Execute the full state machine until run or fault state.
 */
void DevM_MainFunction(void *params)
{
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
