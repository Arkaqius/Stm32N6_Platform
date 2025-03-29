/**
 * @file DevM_PostOS.c
 * @brief Implementation of post-OS initialization for the DevM module.
 *
 * This file contains the implementation of functions required to initialize
 * various components of the DevM module after the operating system has started.
 */

/* Includes -----------------------------------------------------------------*/
#include "DevM.h"
/* Defines ------------------------------------------------------------------*/

/* Local Types and Typedefs -------------------------------------------------*/

/* Global Variables ---------------------------------------------------------*/

/* Private Function Prototypes ----------------------------------------------*/
static DevM_ReturnType DevM_StateInitBswPostOS(void);
static DevM_ReturnType DevM_StateInitMiddlewarePostOS(void);
static DevM_ReturnType DevM_StateInitServicesPostOS(void);
static DevM_ReturnType DevM_StateInitApp(void);
/* Public Functions Implementation ------------------------------------------*/
/**
 * @brief Initialize all post-OS components.
 * @return DEVM_OK if successful.
 */
DevM_ReturnType DevM_StateInitPostOS(void)
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

/* Private Functions Implementation -----------------------------------------*/
static DevM_ReturnType DevM_StateInitBswPostOS(void) { return DEVM_OK; }
static DevM_ReturnType DevM_StateInitMiddlewarePostOS(void) { return DEVM_OK; }
static DevM_ReturnType DevM_StateInitServicesPostOS(void) { return DEVM_OK; }
static DevM_ReturnType DevM_StateInitApp(void) { return DEVM_OK; }