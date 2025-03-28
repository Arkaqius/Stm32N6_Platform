/**
 ********************************************************************************
 * @file    ${file_name}
 * @author  ${user}
 * @date    ${date}
 * @brief
 ********************************************************************************
 */

#ifndef ECUM_H
#define ECUM_H

/************************************
 * INCLUDES
 ************************************/
#include <stdbool.h>
/************************************
 * MACROS AND DEFINES
 ************************************/

/************************************
 * TYPEDEFS
 ************************************/
typedef struct DevManager_T DevManager_T;
/************************************
 * EXPORTED VARIABLES
 ************************************/
bool DevManager_init(void);
DevManager_T *DevManager_getInstance(void);
void DevM_RunStateMachine(void);
/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/

#endif /* ECUM_H */