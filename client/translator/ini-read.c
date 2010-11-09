#include <stdlib.h>
#include "translator.h"

translator_settings_t *default_settings = NULL;

int translator_read_ini(char *config_file)
{
    GKeyFile *keyfile;
    GKeyFileFlags flags;
    GError *error = NULL;
    gsize length;

    
    // read the translator ini file
    keyfile = g_key_file_new();
    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

    // Load the GKeyFile from keyfile.conf or return
    //g_debug("Loading configuration from keyfile.");
    if(!g_key_file_load_from_file(keyfile, config_file, flags, &error))
    {
        g_error("%s", error->message);
        return 1;
    }

    // Create the default settings struct
    //g_debug("Creating default settings struct.");
    default_settings = g_slice_new0(translator_settings_t);
    if(NULL == default_settings)
    {
        g_error("translator_read_ini: Failed to get a slice for the settings.");
        return 1;
    }

    // Collect state resource names for DMM HAB
    default_settings->state_names = g_key_file_get_string_list(keyfile, "Translator",
                                             "dmm_state_names", &length, &error); 
    if((NULL == default_settings->state_names) || (NUM_ADCS != length))
    {
        g_error("translator_read_ini: failed to get resource state names from the file %s.",
                                                                  config_file);
        return 1;
    }

    // Collect calbration constant resources names for  DMM HAB
    default_settings->dmm_calibrations = g_key_file_get_string_list(keyfile, "Translator",
                                                     "dmm_calibrations", &length, &error);
    if((NULL == default_settings->dmm_calibrations) || (NUM_DMM_CALIBRATIONS != length))
    {
        g_error("translator_read_ini: failed to get adc_calibration from te file %s.",
                                                            config_file);
        return 1;
    }

     // Collect translator adc resource names
    default_settings->translator_adc = g_key_file_get_string_list(keyfile, "Translator",
                                                       "translator_adc", &length, &error);
    if((NULL == default_settings->translator_adc) || (NUM_ADCS != length))
    {
        g_error("translator_read_ini: failed to get cali consts from the file %s.",
                                                         config_file);
        return 1;
    }

     // Collect proxr adc resource names
    default_settings->proxr_adc = g_key_file_get_string_list(keyfile, "Translator",
                                                       "proxr_adc", &length, &error);
    if((NULL == default_settings->proxr_adc) || (NUM_ADCS != length))
    {
        g_error("translator_read_ini: failed to get cali consts from the file %s.",
                                                         config_file);
        return 1;
    }

    return 0;
}
