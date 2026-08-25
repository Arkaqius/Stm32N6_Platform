
typedef enum Symptoms_Tag
{
    SYMPTOM_NONE = 0, /**< No symptom */
    SYMPTOM_NULLPTR,  /**< Null pointer access */
    SYMPTOM_TOTAL     /**< Total number of symptoms */
} SymptomsEnum_T;

#define SYS_SYMPTOMS_CFG           \
    {                              \
        [SYMPTOM_NULLPTR] = {      \
            .id = SYMPTOM_NULLPTR, \
        }                          \
    }