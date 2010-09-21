#include "pa-config.h"
#include <stdlib.h>
#include <glib.h>

pa_settings_t *default_settings = NULL;

int pa_read_ini(char *config_file)
{
    GKeyFile *keyfile;
    GKeyFileFlags flags;
    GError *error = NULL;
    gsize length;

    
    // read the proxr-arduino ini file
    keyfile = g_key_file_new();
    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

    // Load the GKeyFile from keyfile.conf or return
    g_debug("Loading configuration from keyfile.");
    if(!g_key_file_load_from_file(keyfile, config_file, flags, &error))
    {
        g_error("%s", error->message);
        return 1;
    }

    // Create the default settings struct
    g_debug("Creating default settings struct.");
    default_settings = g_slice_new0(pa_settings_t);
    if(NULL == default_settings)
    {
        g_error("pa_read_ini: Failed to get a slice for the settings.");
        return 1;
    }

    // Collect dial tooltip Names
    g_debug("Collecting dial names.");
    default_settings->dial_names = g_key_file_get_string_list(keyfile, "PA", "Names", &length, &error);    if((NULL == default_settings->dial_names) || (PA_NUM_NAMES != length))
    {
        g_error("pa_read_ini: failed to get Name settings from the file %s. %s", config_file, error->message);
        return 1;
    }

     // Collect mins_names resource names 
    default_settings->mins_names = g_key_file_get_string_list(keyfile, "PA", "translator_mins", &length, &error);
    if((NULL == default_settings->mins_names) || (PA_NUM_NAMES != length))
    {
        g_error("pa_read_ini: failed to get mins_names names from file %s. %s", config_file, error->message);
        return 1;
    }
    
    // Collect maxs_names resource names
    default_settings->maxs_names = g_key_file_get_string_list(keyfile, "PA", "translator_maxs", &length, &error);
    if((NULL == default_settings->maxs_names) || (PA_NUM_NAMES != length))
    {
        g_error("pa_read_ini: failed to get maxs_names names from file %s. %s", config_file, error->message);
        return 1;
    }

    // Collect arduino-hab resource names
    g_debug("Collecting adc resources.");
    default_settings->arduino = g_key_file_get_string_list(keyfile, "PA", "Arduino", &length, &error);
    if((NULL == default_settings->arduino) || (PA_NUM_ARDUINO != length))
    {
        g_error("pa_read_ini: failed to get arduino dp's from file %s. %s", config_file, error->message);
        return 1;
    }

    // Collect translators adc-state resource names
    g_debug("Collecting adc-state resources.");
    default_settings->state_names = g_key_file_get_string_list(keyfile, "PA", "state_names", &length, &error);
    if((NULL == default_settings->state_names) || (PA_NUM_NAMES != length))
    {
        g_error("pa_read_ini: Failed to get adc-state names from file %s. %s.", config_file, error->message);
        return 1;
    }

    return 0;
}
