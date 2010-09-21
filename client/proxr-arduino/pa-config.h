#include <glib.h>

#define PA_NUM_NAMES        16
#define PA_NUM_ARDUINO      8

#define ON                  1
#define OFF                 0

typedef struct
{
    gchar **mins_names; // translator clab
    gchar **maxs_names; // translator clab
    gchar **state_names; // translator clab
    gchar **dial_names; // tooltip name in gui
    gchar **arduino;    // arduino resource names
}pa_settings_t;

extern pa_settings_t *default_settings;
extern int cookedMode;

int pa_read_ini(char *config_file);
