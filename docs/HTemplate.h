/**
 * @file example_module.h
 * @brief Brief description of the file purpose
 *
 * Detailed description of the module or file functionality.
 */

#ifndef EXAMPLE_MODULE_H
#define EXAMPLE_MODULE_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* Macros and Defines -------------------------------------------------------*/
#define EXAMPLE_MODULE_CONSTANT (100U)

/* Typedefs -----------------------------------------------------------------*/
/**
 * @brief Example structure description
 */
typedef struct
{
    uint32_t id;
    bool is_active;
} ExampleStruct_t;

/* Exported Variables -------------------------------------------------------*/
extern uint32_t g_exampleGlobalVariable;

/* Exported Interfaces ------------------------------------------------------*/
/**
 * @brief Brief description of example interface function
 *
 * Detailed description of the function behavior.
 *
 * @param[in] param1 Description of first parameter
 * @param[out] result Description of output parameter
 *
 * @return Status code
 */
int32_t Example_Module_DoSomething(uint32_t param1, uint32_t *result);

#endif /* EXAMPLE_MODULE_H */
