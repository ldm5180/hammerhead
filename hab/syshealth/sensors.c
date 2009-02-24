
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


// 
// This syshealth module reads info from the sensors(1) program.  It shells
// out rather than linking against libsensors, so that the rest of the
// syshealth program can run on machines without libsensors installed.
//


#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>

#include <glib.h>

#include <hardware-abstractor.h>

#include "syshealth.h"




typedef struct {
    char *name;
    float value;
} sensor_t;




// 
// Here's what sensors says:
//
// adm1027-i2c-0-2e
// Adapter: SMBus I801 adapter at eda0
// V1.5:      +1.497 V  (min =  +1.41 V, max =  +1.61 V)
// VCore:     +1.304 V  (min =  +1.09 V, max =  +1.63 V)
// V3.3:      +3.283 V  (min =  +3.08 V, max =  +3.52 V)
// V5:       +5.098 V  (min =  +4.66 V, max =  +5.34 V)
// V12:      +11.766 V  (min = +10.50 V, max = +13.50 V)
// CPU_Fan:      0 RPM  (min =    0 RPM)
// fan2:         0 RPM  (min =    0 RPM)
// fan3:         0 RPM  (min =    0 RPM)
// fan4:         0 RPM  (min =    0 RPM)
// CPU Temp: +62.75 C  (low  =  -128 C, high =   +85 C)
// Board Temp:
//           +38.00 C  (low  =  -128 C, high =  +127 C)
// Remote Temp:
//           +33.50 C  (low  =  -128 C, high =   +85 C)
// CPU_PWM:   255
// Fan2_PWM:  255
// Fan3_PWM:  255
// vid:      +1.388 V  (VRM Version 10.0)
//


static void sensors_glog_sensor(uint32_t msg_level, sensor_t *s) {
    if (s == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "sensors_print_sensor(): NULL sensor passed in");
        return;
    }

    if (s->name == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "sensors_print_sensor(): sensor with NULL name passed in");
        return;
    }

    g_log("", msg_level, "[%s] = %f", s->name, s->value);
}


static sensor_t *sensors_new_sensor(float value, const char *fmt, ...) {
    va_list ap;
    int r;
    sensor_t *s;
    char name[200];

    s = (sensor_t *)malloc(sizeof(sensor_t));
    if (s == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "out of memory");
        return NULL;
    }

    va_start(ap, fmt);
    r = vsnprintf(name, sizeof(name), fmt, ap);
    va_end(ap);

    /* If that worked, return the string. */
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "sensors: error getting sensor name: %s", strerror(errno));
        free(s);
        return NULL;
    }

    if (r > sizeof(name)) {
        g_log("", G_LOG_LEVEL_WARNING, "sensors: sensor name too long");
        free(s);
        return NULL;
    }

    s->name = strdup(name);
    if (s->name == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "out of memory");
        free(s);
        return NULL;
    }

    {
        char *p;
        for (p = s->name; *p != '\0'; p ++) {
            if (!isalnum(*p) && (*p != '-')) *p = '-';
        }
    }

    s->value =  value;

    return s;
}




static GQueue *sensors_read(void) {
    char line[1000];
    FILE *f;

    GQueue *sensors;

    sensors = g_queue_new();
    if (sensors == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "out of memory");
        return NULL;
    }

    f = popen("sensors -A 2> /dev/null", "r");
    if (f == NULL) {
	g_log("", G_LOG_LEVEL_WARNING, "sensors: error shelling out: %s", strerror(errno));
        g_queue_free(sensors);
        return NULL;
    }

    do {
        char *p;

        sensor_t *sensor;

        char name[100];
        float value;
        char type[100];
        char colon[2];


        p = fgets(line, sizeof(line), f);
        if (p == NULL) {
            // done
            break;
        }
        g_log("", G_LOG_LEVEL_DEBUG, "sensors: read [%s]", line);

        if (4 == sscanf(line, "%99[^:]%1[:] %f %99[^ ]", name, colon, &value, type)) {
            sensor = sensors_new_sensor(value, "%s_%s", name, type);

        } else if (3 == sscanf(line, "%99[^:]%1[:] %f", name, colon, &value)) {
            sensor = sensors_new_sensor(value, "%s", name);

        } else if (2 == sscanf(line, "%99[^:]%1[:]", name, colon)) {
            p = fgets(line, sizeof(line), f);
            if (p == NULL) {
                // done
                break;;
            }
            g_log("", G_LOG_LEVEL_DEBUG, "sensors: read [%s]", line);

            if (2 == sscanf(line, " %f %99[^ ]", &value, type)) {
                sensor = sensors_new_sensor(value, "%s_%s", name, type);

            } else if (1 == sscanf(line, " %f", &value)) {
                sensor = sensors_new_sensor(value, "%s", name);

            } else {
                continue;
            }

        } else {
            continue;
        }


        if (sensor != NULL) {
            sensors_glog_sensor(G_LOG_LEVEL_DEBUG, sensor);
            g_queue_push_tail(sensors, sensor);
        }
    } while (1);


    pclose(f);


    if (g_queue_get_length(sensors) < 1) {
        g_queue_free(sensors);
        return NULL;
    }


    return sensors;
}


static void sensors_add_resource(gpointer sensor_as_gpointer, gpointer node_as_gpointer) {
    sensor_t *sensor = sensor_as_gpointer;
    bionet_node_t *node = node_as_gpointer;
    bionet_resource_t *resource;
    int r;

    resource = bionet_resource_new(node,
            BIONET_RESOURCE_DATA_TYPE_FLOAT,
            BIONET_RESOURCE_FLAVOR_SENSOR,
            sensor->name);
    if (resource == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "sensors_add_resource(): error creating resource \'%s\'", sensor->name);
        return;
    }

    r = bionet_resource_set_float(resource, sensor->value, NULL);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "sensors_add_resource(): error setting sensor resource '%s'", sensor->name);
        return;
    }
    
    r = bionet_node_add_resource(node, resource);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "sensors_add_resource(): error adding sensor resource '%s'", sensor->name);
        return;
    }
    
    g_log("", G_LOG_LEVEL_DEBUG, "added sensors resource '%s'", sensor->name);
}


static void free_sensor(gpointer sensor, gpointer unused) {
    free(sensor);
}


static void free_g_queue(GQueue *q) {
    g_queue_foreach(q, free_sensor, NULL);
    g_queue_free(q);
}


int sensors_init(bionet_node_t *node) {
    GQueue *sensors;

    sensors = sensors_read();
    if (sensors == NULL) {
        return -1;
    }

    g_queue_foreach(sensors, sensors_add_resource, node);

    free_g_queue(sensors);

    return 0;
}


static void sensors_update_resource(gpointer sensor_as_gpointer, gpointer node_as_gpointer) {
    sensor_t *sensor = sensor_as_gpointer;
    bionet_node_t *node = node_as_gpointer;
    bionet_resource_t *resource;
    int r;

    resource = bionet_node_get_resource_by_id(node, sensor->name);
    if (resource == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "sensor_update_resource(): node unable to find resource name '%s'", sensor->name);
        return;
    }

    r = bionet_resource_set_float(resource, sensor->value, NULL);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "sensor_update_resource(): error updating sensors resource '%s': %s", sensor->name, strerror(errno));
    } else {
        g_log("", G_LOG_LEVEL_DEBUG, "sensor_update_resource(): updated sensors resource '%s' to %f", sensor->name, sensor->value);
    }
}


void sensors_update(bionet_node_t *node) {
    GQueue *sensors;

    sensors = sensors_read();
    if (sensors == NULL) {
        return;
    }

    g_queue_foreach(sensors, sensors_update_resource, node);

    free_g_queue(sensors);
}

