#ifndef CFG_FLT_MGR_H
#define CFG_FLT_MGR_H

/* Includes -----------------------------------------------------------------*/

/** Opaque Fault Manager instance supplied by the configuration layer. */
struct FltMan_Tag;

typedef enum SymptomsEnum_Tag
{
    SYMPTOM_INVALID = 0,         /**< No symptom. */
    SYMPTOM_SW_NULL_PTR,         /**< Null pointer passed or dereferenced. */
    SYMPTOM_SW_INVALID_ARGUMENT, /**< Function argument is invalid. */
    SYMPTOM_SW_OUT_OF_RANGE,     /**< Value or index is outside its valid range. */
    SYMPTOM_SW_INVALID_STATE,    /**< Operation is not valid in the current state. */
    SYMPTOM_SW_CONFIG_ERROR,     /**< Software configuration is inconsistent or invalid. */
    SYMPTOM_SW_INVALID_RESOURCE, /**< Configured resource does not exist, such as an unavailable ADC. */
    SYMPTOM_SW_INIT_FAILURE,     /**< Software component initialization failed. */
    SYMPTOM_SW_TIMEOUT,          /**< Software operation did not complete before its deadline. */
    SYMPTOM_SW_BUFFER_OVERFLOW,  /**< Software attempted to exceed a buffer's capacity. */
    SYMPTOM_SW_ASSERT_FAILURE,   /**< Runtime software invariant or assertion failed. */
    SYMPTOM_TOTAL                /**< Total number of symptoms. */
} SymptomsCfgEnum_T;

typedef enum FaultsEnum_Tag
{
    FAULT_INVALID = 0, /**< No FAULT */
    FAULT_SW_ERROR,    /**< Null pointer access */
    FAULT_TOTAL        /**< Total number of Faults */
} FaultsEnum_T;

extern const FltMan_T g_flt_man;

#endif /* CFG_FLT_MGR_H */
