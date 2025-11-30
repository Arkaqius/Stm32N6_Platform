#include "fault.h"

typedef enum Faults_Tag
{
    SYMPTOM_INVALID = 0, /**< No symptom */
    SYMPTOM_SW_ERROR,    /**< Null pointer access */
    SYMPTOM_TOTAL        /**< Total number of symptoms */
} SymptomsEnum_T;

Fault_T flt = {.}
#define SYS_FAULTS_CFG \
    {
}