/**
 * @file      DevM.c
 * @copyright [Copyright Holder]
 *
 * @brief     Implementation of Device Manager state machine and singleton.
 *            Manages the initialization sequence and lifecycle of the system.
 */

/*******************************************************************************
 * Includes
 *******************************************************************************/
#include "DevM.h"
#include "DevM_PreOS.h"
#include "FreeRTOS.h"
#include "stddef.h"

/*******************************************************************************
 * Defines
 *******************************************************************************/

/*******************************************************************************
 * Local Types and Typedefs
 *******************************************************************************/

/*******************************************************************************
 * Global Variables
 *******************************************************************************/
/**
 * @brief Singleton instance of Device Manager.
 */
static DevManager_T g_DevManager_instance;

/*******************************************************************************
 * Static Function Prototypes
 *******************************************************************************/
static bool DevManager_init(void);
/*******************************************************************************
 * Functions
 *******************************************************************************/

/**
 * @brief Perform Device Manager startup sequence.
 *
 * Initializes the Device Manager, performs necessary Pre-OS and OS initializations,
 * and then starts the FreeRTOS scheduler.
 */
void DevM_Startup(void)
{
  DevManager_init();     /* Initialize Device Manager Singleton */
  DevM_StateInitPreOS(); /* Execute Pre-OS initialization */
  DevM_StateInitOS();    /* Execute OS-specific initialization */
  vTaskStartScheduler(); /* Start FreeRTOS scheduler, dispatch context switching */
}

/**
 * @brief Initialize Device Manager singleton instance.
 *
 * @return true if initialization successful, false otherwise.
 */
static bool DevManager_init(void)
{
  if (!g_DevManager_instance.initialized)
  {
    g_DevManager_instance.initialized = true;
    return true;
  }
  return false;
}

/**
 * @brief Retrieve Device Manager singleton instance.
 *
 * @return Pointer to initialized Device Manager instance, or NULL if not initialized.
 */
DevManager_T *DevManager_getInstance(void)
{
  return g_DevManager_instance.initialized ? &g_DevManager_instance : NULL;
}