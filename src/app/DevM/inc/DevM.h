/**
 * @file      DevM.h
 * @brief     Interface definitions for the Device Manager module.
 *
 * Provides type definitions, enums, and function prototypes related to the
 * Device Manager's lifecycle, state machine control, and singleton pattern.
 */

#ifndef DEVM_H
#define DEVM_H

/* Includes -----------------------------------------------------------------*/
#include "stdint.h"
#include "stdbool.h"

/* Macros and Defines -------------------------------------------------------*/
/** Default initialization value used for DevM example */
#define DEVM_DEFAULT_INIT_VALUE (100U)

/* Typedefs -----------------------------------------------------------------*/

/**
 * @brief Result codes returned by Device Manager functions.
 */
typedef enum
{
    DEVM_OK = 0, /**< Operation successful */
    DEVM_ERROR   /**< Operation failed */
} DevM_ReturnType;

/**
 * @brief State machine states for Device Manager.
 */
typedef enum DevM_StateType
{
    DEVM_STATE_INIT_PRE_OS,  /**< State for initialization before OS startup */
    DEVM_STATE_INIT_OS,      /**< State for OS-specific initialization */
    DEVM_STATE_INIT_POST_OS, /**< State after OS initialization completes */
    DEVM_STATE_RUN,          /**< Normal operational state */
    DEVM_STATE_FAULT,        /**< State when fault detected */
    DEVM_STATE_SOFT_RESTART  /**< State for system soft-reset or restart */
} DevM_StateType;

/**
 * @brief Singleton structure definition for Device Manager.
 */
typedef struct DevManager_T
{
    bool initialized; /**< Flag indicating initialization status */
} DevManager_T;

/* Exported Variables -------------------------------------------------------*/

/* Exported Interfaces ------------------------------------------------------*/

/**
 * @brief Retrieve pointer to Device Manager instance.
 *
 * @return Pointer to singleton Device Manager instance or NULL if uninitialized.
 */
DevManager_T *DevManager_getInstance(void);

/**
 * @brief Execute Device Manager state machine.
 *
 * This function manages transitions and operations according to the Device
 * Manager state machine.
 */
void DevM_RunStateMachine(void);

#endif /* DEVM_H */