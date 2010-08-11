#include <glib.h>

#define PA_NUM_MINIMUMS     16
#define PA_NUM_MAXIMUMS     16
#define PA_NUM_INCREMENTS   16

typedef struct
{
    double *minimums;
    double *maximums;
    double *increments;
} pa_settings_t;

extern pa_settings_t *default_settings;

