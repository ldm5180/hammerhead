
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


static int mkpath_for_resource(bionet_resource_t * resource, char * persist_dir);


int bionet_resource_persist(bionet_resource_t * resource, char * persist_dir) {
    char hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
    char hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char path[(BIONET_NAME_COMPONENT_MAX_LEN * 4) + 256];
    int fd, r, i, so_far;
    FILE * fp;
    char * buf = NULL;
    bionet_datapoint_t * dp;
    char timestamp_str[256];
    struct timeval tv;

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

    dp = BIONET_RESOURCE_GET_DATAPOINT(resource);
    if (dp) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	      "Resource %s already has a datapoint. Using that one instead of the persisted one.",
	      bionet_resource_get_name(resource));
	goto exit1;
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
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	      "bionet_resource_persist: unable to open persist file %s for %s, moving on. %m",
	      path, bionet_resource_get_name(resource));
	goto exit1;
    }

    /* must have opened the file, so switch it to a stream */
    fp = fdopen(fd, "r"); 
    if (NULL == fp) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	      "bionet_resource_persist: unable to fdopen persist file %s for %s, moving on. %m",
	      path, bionet_resource_get_name(resource));
	close(fd);
	goto exit1;
    }

    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	  "bionet_resource_persist: fdopened persist file %s for %s, reading.",
	  path, bionet_resource_get_name(resource));

    /* read the timestamp */
    if (NULL == fgets(timestamp_str, sizeof(timestamp_str), fp)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	      "bionet_resource_persist: No valid timestamp string found in file %s: %m",
	      path);
	fclose(fp);
	return 1;
    }
    
    if (2 != sscanf(timestamp_str, "%lu.%lu", &tv.tv_sec, &tv.tv_usec)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	      "bionet_resource_persist: Unable to parse timestamp string to timeval %s: %m",
	      timestamp_str);
	fclose(fp);
	return 1;
    }

    /* read the rest of file into a big buffer, hopefully it isn't too big */
    i = 0;
    so_far = 0;
    do {
	buf = realloc(buf, so_far + (512 * ++i) + 1);
	if (NULL == buf) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: Out of memory - %m");
	    fclose(fp);
	    return 1;
	}

	so_far += fread(&buf[so_far], 1, 512, fp);
	if (ferror(fp)) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_resource_persist: fread error - %m");
	    fclose(fp);
	    free(buf);
	    return 1;
	}
    } while (0 == feof(fp));

    buf[so_far] = '\0';

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
	} else if (bionet_resource_set_binary(resource, (int)val, &tv)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_resource_persist: Failed to set binary resource %s to %d",
		      bionet_resource_get_name(resource), (int)val);
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
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
	} else if (bionet_resource_set_uint8(resource, (uint8_t)val, &tv)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_resource_persist: Failed to set uint8 resource %s to %u",
		      bionet_resource_get_name(resource), (uint8_t)val);
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
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
	} else if (bionet_resource_set_int8(resource, (int8_t)val, &tv)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_resource_persist: Failed to set int8 resource %s to %d",
		      bionet_resource_get_name(resource), (int8_t)val);
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
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
	} else if (bionet_resource_set_uint16(resource, (uint16_t)val, &tv)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_resource_persist: Failed to set uint16 resource %s to %u",
		      bionet_resource_get_name(resource), (uint16_t)val);
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
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
	} else if (bionet_resource_set_int16(resource, (int16_t)val, &tv)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_resource_persist: Failed to set int16 resource %s to %d",
		      bionet_resource_get_name(resource), (int16_t)val);
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
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
	} else if (bionet_resource_set_uint32(resource, (uint32_t)val, &tv)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_resource_persist: Failed to set uint32 resource %s to %u",
		      bionet_resource_get_name(resource), (uint32_t)val);
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
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
	} else if (bionet_resource_set_int32(resource, (int32_t)val, &tv)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_resource_persist: Failed to set int32 resource %s to %d",
		      bionet_resource_get_name(resource), (int32_t)val);
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
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
	} else if (bionet_resource_set_float(resource, val, &tv)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_resource_persist: Failed to set float resource %s to %f",
		      bionet_resource_get_name(resource), val);
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
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
	} else if (bionet_resource_set_double(resource, val, &tv)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_resource_persist: Failed to set double resource %s to %f",
		      bionet_resource_get_name(resource), val);
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
		  "bionet_resource_persist: Successfully set resource %s to stored value %f",
		  bionet_resource_get_name(resource), val);
	}
	break;
    }

    case BIONET_RESOURCE_DATA_TYPE_STRING:
	if (bionet_resource_set_str(resource, buf, &tv)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_resource_persist: Failed to set double resource %s to %s",
		      bionet_resource_get_name(resource), buf);
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
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

    if (mkpath_for_resource(resource, persist_dir)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "bionet_resource_write_persist: Failed to create directory structure for resource %s",
	      bionet_resource_get_name(resource));
	return 1;	
    }

    return 0;
}


static int mkpath_for_resource(bionet_resource_t * resource, char * persist_dir) {
    char hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
    char hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char dirpath[(BIONET_NAME_COMPONENT_MAX_LEN * 4) + 256];
    char path[(BIONET_NAME_COMPONENT_MAX_LEN * 4) + 256];
    char newpath[(BIONET_NAME_COMPONENT_MAX_LEN * 4) + 256];
    int r;
    //int fd;
    FILE * fp;
    bionet_datapoint_t * dp;
    char * val;
    char timestamp_str[256];
    struct timeval * tv;

    if (bionet_split_resource_name_r(bionet_resource_get_name(resource),
				   hab_type, hab_id, node_id, resource_id)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "mkpath_for_resource: Unable to split resource name %s", 
	      bionet_resource_get_name(resource));
	return 1;
    }

    /* create the persist_dir */
    r = mkdir(persist_dir, 0755);
    if ((0 != r) && (EEXIST != errno)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "mkpath_for_resource: unable to dir %s : %m",
	      persist_dir);
	return 1;
    }

    /* create the persist_dir/hab-type */
    r = snprintf(dirpath, sizeof(dirpath), "%s/%s",
		 persist_dir, hab_type);
    if ((r >= sizeof(dirpath)) || (r < 0)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "mkpath_for_resource: unable to create persist dir for resource %s and path %s for hab type",
	      bionet_resource_get_name(resource), persist_dir);
	return 1;
    }
    r = mkdir(dirpath, 0755);
    if ((0 != r) && (EEXIST != errno)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "mkpath_for_resource: unable to dir %s : %m",
	      dirpath);
	return 1;
    }

    /* create the persist_dir/hab-type/hab-id */
    r = snprintf(dirpath, sizeof(dirpath), "%s/%s/%s",
		 persist_dir, hab_type, hab_id);
    if ((r >= sizeof(dirpath)) || (r < 0)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "mkpath_for_resource: unable to create persist dir for resource %s and path %s for hab id",
	      bionet_resource_get_name(resource), persist_dir);
	return 1;
    }
    r = mkdir(dirpath, 0755);
    if ((0 != r) && (EEXIST != errno)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "mkpath_for_resource: unable to dir %s : %m",
	      dirpath);
	return 1;
    }

    /* create the persist_dir/hab-type/hab-id/node-id */
    r = snprintf(dirpath, sizeof(dirpath), "%s/%s/%s/%s",
		 persist_dir, hab_type, hab_id, node_id);
    if ((r >= sizeof(dirpath)) || (r < 0)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "mkpath_for_resource: unable to create persist dir for resource %s and path %s for node id",
	      bionet_resource_get_name(resource), persist_dir);
	return 1;
    }
    r = mkdir(dirpath, 0755);
    if ((0 != r) && (EEXIST != errno)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "mkpath_for_resource: unable to dir %s : %m",
	      dirpath);
	return 1;
    }

    /* create the file name persist_dir/hab-type/hab-id/node-id/resource-id.TYPE-new */
    r = snprintf(newpath, sizeof(newpath), "%s/%s/%s/%s/%s.%s-new",
		 persist_dir, hab_type, hab_id, node_id, resource_id, 
		 bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)));
    if ((r >= sizeof(newpath)) || (r < 0)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "mkpath_for_resource: unable to create persist new path for resource %s and path %s for resource",
	      bionet_resource_get_name(resource), persist_dir);
	return 1;
    }

    /* create the file name persist_dir/hab-type/hab-id/node-id/resource-id.TYPE */
    r = snprintf(path, sizeof(path), "%s/%s/%s/%s/%s.%s",
		 persist_dir, hab_type, hab_id, node_id, resource_id, 
		 bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)));
    if ((r >= sizeof(path)) || (r < 0)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "mkpath_for_resource: unable to create persist path for resource %s and path %s",
	      bionet_resource_get_name(resource), persist_dir);
	return 1;
    }

    
#if 0
    fd = open(newpath, O_SYNC | O_CREAT | O_TRUNC | O_WRONLY); 
    if (-1 == fd) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "mkpath_for_resource: unable to create persist file for resource %s and path %s",
	      bionet_resource_get_name(resource), newpath);
	return 1;
    }
#endif

    fp = fopen(newpath, "w");
    if (NULL == fp) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	      "mkpath_for_resource: unable to fdopen persist file %s for %s, moving on. %m",
	      path, bionet_resource_get_name(resource));
	return 1;
    }

    dp = BIONET_RESOURCE_GET_DATAPOINT(resource);
    if (NULL == dp) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "mkpath_for_resource: resource %s has no datapoints",
	      bionet_resource_get_name(resource));
	goto exit2;
    }

    tv = bionet_datapoint_get_timestamp(dp);
    if (sizeof(timestamp_str) <= snprintf(timestamp_str, sizeof(timestamp_str), 
					  "%lu.%lu\n", tv->tv_sec, tv->tv_usec)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "mkpath_for_resource: timestamp string is too long for resource %s",
	      bionet_resource_get_name(resource));
	goto exit2;
    }
    if (1 != fwrite(timestamp_str, strlen(timestamp_str), 1, fp)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "mkpath_for_resource: Failed to write timestamp for resource %s data to file %s",
	      bionet_resource_get_name(resource),
	      timestamp_str);
	goto exit2;
    }
    val = bionet_value_to_str(bionet_datapoint_get_value(dp));
    if (1 != fwrite(val, strlen(val), 1, fp)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "mkpath_for_resource: Failed to write resource %s data to file %s",
	      bionet_resource_get_name(resource),
	      val);
	goto exit3;
    }

exit3:
    free(val);

exit2:
    fclose(fp);

    if (rename(newpath, path)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	      "mkpath_for_resource: Failed to rename file to %s", path);
	return 0;
    }

    return 0;
} /* mkpath_for_resource() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
