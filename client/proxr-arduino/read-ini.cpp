#include "pa-config.h"

pa_settings_t *default_settings = NULL;

int pa_read_ini(char *config_file)
{
    GKeyFile *keyfile;
//    GKeyFileFlags flags;
    GError *error = NULL;
    gsize length;

    // read the proxr-arduino ini file
    keyfile = g_key_file_new();
 //   flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

    // Load the GKeyFile from keyfile.conf or return
    g_debug("Loading configuration from keyfile.");
    if(!g_key_file_load_from_file(keyfile, config_file, (GKeyFileFlags)0, &error))
    {
        g_error("%s", error->message);
        return 1;
    }

    // Create the default settings struct
    default_settings = g_slice_new0(pa_settings_t);
    if(default_settings = NULL)
    {
        g_error("pa_read_ini: Failed to get a slice for the settings.");
        return 1;
    }

    // Collect defaults for Minimums
    default_settings->minimums = g_key_file_get_double_list(keyfile, "PA", "Minimums",
                                                              &length, &error);
    if((default_settings->minimums == NULL) || (PA_NUM_MINIMUMS != length))
    {
        g_error("pa_read_ini: failed to get Minimum settings from the file %s. %s", config_file, error->message);
        return 1;
    }

    // Collect defaults for Maximums
    default_settings->maximums = g_key_file_get_double_list(keyfile, "PA", "Maximums",
                                                             &length, &error);
    if((default_settings->maximums == NULL) || (PA_NUM_MAXIMUMS != length))
    {
        g_error("pa_read_ini: failed to get Maximum settings from the file %s. %s", config_file, error->message);
        return 1;
    }

    // Collect defaults for increments
    default_settings->increments = g_key_file_get_double_list(keyfile, "PA", "Increments",                                                              &length, &error);
    if((default_settings->increments == NULL) || (PA_NUM_INCREMENTS != length))
    {
        g_error("pa_read_ini: failed to get Increment settings from the file %s. %s", config_file, error->message);
        return 1;
    }
}
