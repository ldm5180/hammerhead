
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#include "bionet-util.h"
#include "protected.h"
#include "internal.h"


//static int mkpath_for_resource(bionet_resource_t * resource);


int bionet_resource_persist(bionet_resource_t * resource, char * persist_dir) {
    char hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
    char hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char path[(BIONET_NAME_COMPONENT_MAX_LEN * 4) + 256];
    int fd, r, i, so_far;
    FILE * fp;
    char * buf = NULL;

    /* sanity check */
    if (NULL == resource) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_persist: NULL resource passed in.");
	return 1;
    }

    if (NULL == persist_dir) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "bionet_resource_persist: NULL persist_dir passed in.");
	return 1;
    }


    if (bionet_split_resource_name_r(bionet_resource_get_name(resource),
				     hab_type, hab_id, node_id, resource_id)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "bionet_resource_persist: Unable to split resource name %s", 
	      bionet_resource_get_name(resource));
	return 1;
    }

    /* create the name of the file this should be persisted in */
    r = snprintf(path, sizeof(path), "%s/%s/%s/%s/%s.%s",
		 persist_dir, hab_type, hab_id, node_id, resource_id, 
		 bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)));
    if ((r >= sizeof(path)) || (r < 0)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_persist: unable to create persist path for resource %s and path %s",
	      bionet_resource_get_name(resource), persist_dir);
	return 1;
    }

    /* try to open the file without creating it */
    fd = open(path, O_RDONLY);
    if (-1 == fd) {
	/* didn't open the file so let's get outta here without setting the value */
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, 
	      "bionet_resource_persist: unable to open persist file %s for %s, moving on. %m",
	      path, bionet_resource_get_name(resource));
	goto exit1;
    }

    /* must have opened the file, so switch it to a stream */
    fp = fdopen(fd, "r"); 
    if (NULL == fp) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, 
	      "bionet_resource_persist: unable to fdopen persist file %s for %s, moving on. %m",
	      path, bionet_resource_get_name(resource));
	close(fd);
	goto exit1;
    }

    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, 
	  "bionet_resource_persist: fdopened persist file %s for %s, reading.",
	  path, bionet_resource_get_name(resource));

    /* read the entire file into a big buffer, hopefully it isn't too big */
    i = 0;
    so_far = 0;
    do {
	buf = realloc(buf, so_far + (512 * ++i));
	if (NULL == buf) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Out of memory - %m");
	    fclose(fp);
	    return 1;
	}

	so_far += fread(&buf[so_far], 512, 1, fp);
	if (ferror(fp)) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: fread error - %m");
	    fclose(fp);
	    free(buf);
	    return 1;
	}
    } while (0 == feof(fp));

    fclose(fp);
    
    /* depending on the datatype of the resource, convert the string in the file to that
     * type and set the resource's value */
    errno = 0;
    switch(bionet_resource_get_data_type(resource)) {

    case BIONET_RESOURCE_DATA_TYPE_BINARY:
    {
	char * endptr;
	long int val = strtol(buf, &endptr, 10);
	if (ERANGE == errno && ((LONG_MIN == val) || (LONG_MAX == val))) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file is out of range.");
	} else if (EINVAL == errno) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file cannot be converted.");
	} else if ('\0' != *endptr) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file is invalid.");
	} else if (bionet_resource_set_binary(resource, (int)val, NULL)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_resource_persist: Failed to set binary resource %s to %d",
		      bionet_resource_get_name(resource), (int)val);
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO,
		  "bionet_resource_persist: Successfully set resource %s to stored value %d",
		  bionet_resource_get_name(resource), (int)val);
	}
	break;
    }

    case BIONET_RESOURCE_DATA_TYPE_UINT8:
    {
	char * endptr;
	unsigned long int val = strtoul(buf, &endptr, 10);
	if (ERANGE == errno && ULONG_MAX == val) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file is out of range.");
	} else if (EINVAL == errno) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file cannot be converted.");
	} else if ('\0' != *endptr) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file is invalid.");
	} else if (bionet_resource_set_uint8(resource, (uint8_t)val, NULL)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_resource_persist: Failed to set uint8 resource %s to %u",
		      bionet_resource_get_name(resource), (uint8_t)val);
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO,
		  "bionet_resource_persist: Successfully set resource %s to stored value %u",
		  bionet_resource_get_name(resource), (uint8_t)val);
	}
	break;
    }

    case BIONET_RESOURCE_DATA_TYPE_INT8:
    {
	char * endptr;
	long int val = strtol(buf, &endptr, 10);
	if (ERANGE == errno && ((LONG_MIN == val) || (LONG_MAX == val))) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file is out of range.");
	} else if (EINVAL == errno) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file cannot be converted.");
	} else if ('\0' != *endptr) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file is invalid.");
	} else if (bionet_resource_set_int8(resource, (int8_t)val, NULL)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_resource_persist: Failed to set int8 resource %s to %d",
		      bionet_resource_get_name(resource), (int8_t)val);
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO,
		  "bionet_resource_persist: Successfully set resource %s to stored value %d",
		  bionet_resource_get_name(resource), (int8_t)val);
	}
	break;
    }

    case BIONET_RESOURCE_DATA_TYPE_UINT16:
    {
	char * endptr;
	unsigned long int val = strtoul(buf, &endptr, 10);
	if (ERANGE == errno && ULONG_MAX == val) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file is out of range.");
	} else if (EINVAL == errno) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file cannot be converted.");
	} else if ('\0' != *endptr) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file is invalid.");
	} else if (bionet_resource_set_uint16(resource, (uint16_t)val, NULL)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_resource_persist: Failed to set uint16 resource %s to %u",
		      bionet_resource_get_name(resource), (uint16_t)val);
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO,
		  "bionet_resource_persist: Successfully set resource %s to stored value %u",
		  bionet_resource_get_name(resource), (uint16_t)val);
	}
	break;
    }

    case BIONET_RESOURCE_DATA_TYPE_INT16:
    {
	char * endptr;
	long int val = strtol(buf, &endptr, 10);
	if (ERANGE == errno && ((LONG_MIN == val) || (LONG_MAX == val))) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file is out of range.");
	} else if (EINVAL == errno) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file cannot be converted.");
	} else if ('\0' != *endptr) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file is invalid.");
	} else if (bionet_resource_set_int16(resource, (int16_t)val, NULL)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_resource_persist: Failed to set int16 resource %s to %d",
		      bionet_resource_get_name(resource), (int16_t)val);
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO,
		  "bionet_resource_persist: Successfully set resource %s to stored value %d",
		  bionet_resource_get_name(resource), (int16_t)val);
	}
	break;
    }

    case BIONET_RESOURCE_DATA_TYPE_UINT32:
    {
	char * endptr;
	unsigned long int val = strtoul(buf, &endptr, 10);
	if (ERANGE == errno && ULONG_MAX == val) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file is out of range.");
	} else if (EINVAL == errno) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file cannot be converted.");
	} else if ('\0' != *endptr) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file is invalid.");
	} else if (bionet_resource_set_uint32(resource, (uint32_t)val, NULL)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_resource_persist: Failed to set uint32 resource %s to %u",
		      bionet_resource_get_name(resource), (uint32_t)val);
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO,
		  "bionet_resource_persist: Successfully set resource %s to stored value %u",
		  bionet_resource_get_name(resource), (uint32_t)val);
	}
	break;
    }

    case BIONET_RESOURCE_DATA_TYPE_INT32:
    {
	char * endptr;
	long int val = strtol(buf, &endptr, 10);
	if (ERANGE == errno && ((LONG_MIN == val) || (LONG_MAX == val))) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file is out of range.");
	} else if (EINVAL == errno) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file cannot be converted.");
	} else if ('\0' != *endptr) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file is invalid.");
	} else if (bionet_resource_set_int32(resource, (int32_t)val, NULL)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_resource_persist: Failed to set int32 resource %s to %d",
		      bionet_resource_get_name(resource), (int32_t)val);
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO,
		  "bionet_resource_persist: Successfully set resource %s to stored value %d",
		  bionet_resource_get_name(resource), (int32_t)val);
	}
	break;
    }

    case BIONET_RESOURCE_DATA_TYPE_FLOAT:
    {
	char * endptr;
	float val = strtof(buf, &endptr);
	if (ERANGE == errno && ((-HUGE_VAL == val) || (HUGE_VAL == val))) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file is out of range.");
	} else if ('\0' != *endptr) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file is invalid.");
	} else if (bionet_resource_set_float(resource, val, NULL)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_resource_persist: Failed to set float resource %s to %f",
		      bionet_resource_get_name(resource), val);
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO,
		  "bionet_resource_persist: Successfully set resource %s to stored value %f",
		  bionet_resource_get_name(resource), val);
	}
	break;
    }

    case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
    {
	char * endptr;
	double val = strtod(buf, &endptr);
	if (ERANGE == errno && ((-HUGE_VAL == val) || (HUGE_VAL == val))) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file is out of range.");
	} else if ('\0' != *endptr) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Value in file is invalid.");
	} else if (bionet_resource_set_double(resource, val, NULL)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_resource_persist: Failed to set double resource %s to %f",
		      bionet_resource_get_name(resource), val);
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO,
		  "bionet_resource_persist: Successfully set resource %s to stored value %f",
		  bionet_resource_get_name(resource), val);
	}
	break;
    }

    case BIONET_RESOURCE_DATA_TYPE_STRING:
	if (bionet_resource_set_str(resource, buf, NULL)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_resource_persist: Failed to set double resource %s to %s",
		      bionet_resource_get_name(resource), buf);
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO,
		  "bionet_resource_persist: Successfully set resource %s to stored value %s",
		  bionet_resource_get_name(resource), buf);
	}
	break;

    default:
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_persist: Unknown resource data type %d for %s",
	      bionet_resource_get_data_type(resource),
	      bionet_resource_get_name(resource));
        free(buf);
	return 1;
    }

    free(buf);

exit1:
    resource->persist = 1;

    return 0;
} /* bionet_resource_persist() */


int bionet_resource_is_persisted(const bionet_resource_t * resource) {
    if (NULL == resource) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_is_persisted: NULL resource passed in.");
	return 0;
    }

    return resource->persist;
} /* bionet_resource_is_persisted() */


int bionet_resource_write_persist(bionet_resource_t * resource, char * persist_dir) {
    if (NULL == resource) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_write_persist: NULL resource passed in.");
	return 1;
    }

    if (NULL == persist_dir) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "bionet_resource_write_persist: NULL persist_dir passed in.");
	return 1;
    }

    //TODO
    return 0;
}

#if 0
static int mkpath_for_resource(bionet_resource_t * resource) {
    char * hab_type;
    char * hab_id;
    char * node_id;
    char * resource_id;

    if (bionet_split_resource_name(bionet_resource_get_name(resource),
				   hab_type, hab_id, node_id, resource_id)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "bionet_resource_persist: Unable to split resource name %s", 
	      bionet_resource_get_name(resource));
	return 1;
    }

    r = snprintf(newpath, sizeof(newpath), "%s/%s/%s/%s/%s.%s-new",
		 persist_dir, hab_type, hab_id, node_id, resource_id, 
		 bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)));
    if ((r >= sizeof(newpath)) || (r < 0)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_persist: unable to create persist path for resource %s and path %s",
	      bionet_resource_get_name(resource), persist_dir);
	return 1;
    }

    r = snprintf(path, sizeof(path), "%s/%s/%s/%s/%s.%s",
		 persist_dir, hab_type, hab_id, node_id, resource_id, 
		 bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)));
    if ((r >= sizeof(path)) || (r < 0)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_persist: unable to create persist path for resource %s and path %s",
	      bionet_resource_get_name(resource), persist_dir);
	return 1;
    }

    fd = open(path, O_SYNC | O_CREAT | O_TRUNC | O_WRONLY); 

    return 0;
} /* mkpath_for_resource() */
#endif


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
