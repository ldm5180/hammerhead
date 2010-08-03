
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

// This library is free software. You can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as 
// published by the Free Software Foundation, version 2.1 of the License.
// This library is distributed in the hope that it will be useful, but 
// WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details. A copy of the GNU 
// Lesser General Public License v 2.1 can be found in the file named 
// “COPYING.LESSER”.  You should have received a copy of the GNU Lesser 
// General Public License along with this library; if not, write to the 
// Free Software Foundation, Inc., 
// 51 Franklin Street, Fifth Floor, 
// Boston, MA 02110-1301 USA.
 
// You may contact the Automation Group at:
// bionet@bioserve.colorado.edu
 
// Dr. Kevin Gifford
// University of Colorado
// Engineering Center, ECAE 1B08
// Boulder, CO 80309
 
// Because BioNet was developed at a university, we ask that you provide
// attribution to the BioNet authors in any redistribution, modification, 
// work, or article based on this library.
 
// You may contribute modifications or suggestions to the University of
// Colorado for the purpose of discussing and improving this software.
// Before your modifications are incorporated into the master version 
// distributed by the University of Colorado, we must have a contributor
// license agreement on file from each contributor. If you wish to supply
// the University with your modifications, please join our mailing list.
// Instructions can be found on our website at 
// http://bioserve.colorado.edu/developers-corner.

#ifndef __BIONET_VALUE_H
#define __BIONET_VALUE_H

/**
 * @file bionet-value.h
 * Functions for dealing with Bionet Values.
 */


#include "bionet-platform.h"

#include "libbionet-util-decl.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Get the value's associated resource
 *
 * @param[in] value The value
 *
 * @return Pointer to a resource
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_resource_t * bionet_value_get_resource(bionet_value_t * value);


/**
 * @brief Get the node a value belongs to
 *
 * @param[in] value The value
 *
 * @return Pointer to a Node
 * @return NULL Error
 *
 * @note This is shorthand for getting the resource, then getting the 
 * node of the resource.
 * @code
 * bionet_resource_get_node(bionet_value_get_resource(value))
 * @endcode
 */
BIONET_UTIL_API_DECL
bionet_node_t * bionet_value_get_node(const bionet_value_t * value);


/**
 * @brief Get the HAB a value belongs to
 *
 * @param[in] value The value
 * 
 * @return Pointer to a HAB
 * @return NULL Error
 *
 * @note This is shorthand for getting the resource, then getting the
 * node of the resource, then getting the HAB of the node.
 * @code
 * bionet_node_get_hab(bionet_resource_get_node(bionet_value_get_resource(value)))
 * @endcode
 */
BIONET_UTIL_API_DECL
bionet_hab_t * bionet_value_get_hab(const bionet_value_t * value);


/**
 * @brief Get the datapoint a value belongs to
 * 
 * @param[in] value The value
 *
 * @return Pointer to a datapoint
 * @return NULL This is a stand-alone value or Error
 *
 * @note This is shorthand for getting the resource, then getting the
 * datapoint of the resource.
 * @code
 * bionet_resource_get_get_datapoint_by_index(bionet_value_get_resource(value), 0)
 * @endcode
 */
BIONET_UTIL_API_DECL
bionet_datapoint_t * bionet_value_get_datapoint(const bionet_value_t * value);


/**
 * @brief Allocates and initializes a new value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] resource The Resource that the new value is for
 * @param[in] content The content
 *                  
 * @return The new Value
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_value_t *bionet_value_new_binary(const bionet_resource_t *resource,
					int content);


/**
 * @brief Allocates and initializes a new value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] resource The Resource that the new value is for
 * @param[in] content The content
 *                  
 * @return The new Value
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_value_t *bionet_value_new_uint8(const bionet_resource_t *resource,
				       uint8_t content);


/**
 * @brief Allocates and initializes a new value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] resource The Resource that the new value is for
 * @param[in] content The content
 *                  
 * @return The new Value
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_value_t *bionet_value_new_int8(const bionet_resource_t *resource,
				      int8_t content);


/**
 * @brief Allocates and initializes a new value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] resource The Resource that the new value is for
 * @param[in] content The content
 *                  
 * @return The new Value
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_value_t *bionet_value_new_uint16(const bionet_resource_t *resource,
					uint16_t content);


/**
 * @brief Allocates and initializes a new value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] resource The Resource that the new value is for
 * @param[in] content The content
 *                  
 * @return The new Value
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_value_t *bionet_value_new_int16(const bionet_resource_t *resource,
				       int16_t content);


/**
 * @brief Allocates and initializes a new value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] resource The Resource that the new value is for
 * @param[in] content The content
 *                  
 * @return The new Value
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_value_t *bionet_value_new_uint32(const bionet_resource_t *resource,
					uint32_t content);



/**
 * @brief Allocates and initializes a new value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] resource The Resource that the new value is for
 * @param[in] content The content
 *                  
 * @return The new Value
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_value_t *bionet_value_new_int32(const bionet_resource_t *resource,
				       int32_t content);


/**
 * @brief Allocates and initializes a new value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] resource The Resource that the new value is for
 * @param[in] content The content
 *                  
 * @return The new Value
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_value_t *bionet_value_new_float(const bionet_resource_t *resource,
				       float content);


/**
 * @brief Allocates and initializes a new value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] resource The Resource that the new value is for
 * @param[in] content The content
 *                  
 * @return The new Value
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_value_t *bionet_value_new_double(const bionet_resource_t *resource,
					double content);


/**
 * @brief Allocates and initializes a new value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] resource The Resource that the new value is for
 * @param[in] content The content
 *                  
 * @return The new Value
 * @return NULL Error
 *
 * @note The memory in the content pointer is copied, so the memory
 * pointed to by content is still owned by the caller.
 */
BIONET_UTIL_API_DECL
bionet_value_t *bionet_value_new_str(const bionet_resource_t *resource,
				     const char * content);


/**
 * @brief Frees a Value
 *
 * Value was created with one of the following:
 * bionet_value_new_binary() or bionet_value_new_str()
 * bionet_value_new_uint8() or bionet_value_new_int8() 
 * bionet_value_new_uint16() or bionet_value_new_int16() 
 * bionet_value_new_uint32() or bionet_value_new_int32()
 * bionet_value_new_float() or bionet_value_new_double() 
 *
 * @param[in] value The Value to free.
 */
BIONET_UTIL_API_DECL
void bionet_value_free(bionet_value_t *value);


/**
 * @brief Renders a Value as an ASCII string.
 *
 * @param[in] value The value to create the string from
 *
 * @return A dynamically allocated string containing an ASCII
 *         representation of the Value
 * @return NULL Failure
 *
 * @note The returned string becomes the property (and responsibility) of
 *       the caller.  The caller must free the returned string or leak
 *       memory.
 *
 * @deprecated Use the new bionet_value_as_str() instead.
 */
BIONET_UTIL_API_DECL
char *bionet_value_to_str(const bionet_value_t *value);


/**
 * @brief Renders a Value as an ASCII string.
 *
 * @param[in] value The value to create the string from
 *
 * @return A dynamically allocated string containing an ASCII
 *         representation of the Value
 * @return NULL Failure
 *
 * @note The returned string is property of the bionet_value_t
 * and will be free'd when bionet_value_free() is called.
 */
BIONET_UTIL_API_DECL
char *bionet_value_as_str(bionet_value_t *value);


/**
 * @brief Sets a value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] value The Value that the new content is for
 * @param[in] content The new content of the value
 *
 * @retval 0 Success
 * @retval -1 Error
 */
BIONET_UTIL_API_DECL
int bionet_value_set_binary(bionet_value_t *value,
			    int content);


/**
 * @brief Sets a value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] value The Value that the new content is for
 * @param[in] content The new content of the value
 *                  
 * @retval 0 Success
 * @retval -1 Error
 */
BIONET_UTIL_API_DECL
int bionet_value_set_uint8(bionet_value_t *value,
			   uint8_t content);


/**
 * @brief Sets a value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] value The Value that the new content is for
 * @param[in] content The new content of the value
 *                  
 * @retval 0 Success
 * @retval -1 Error
 */
BIONET_UTIL_API_DECL
int bionet_value_set_int8(bionet_value_t *value,
			  int8_t content);


/**
 * @brief Sets a value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] value The Value that the new content is for
 * @param[in] content The new content of the value
 *                  
 * @retval 0 Success
 * @retval -1 Error
 */
BIONET_UTIL_API_DECL
int bionet_value_set_uint16(bionet_value_t *value,
			    uint16_t content);


/**
 * @brief Sets a value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] value The Value that the new content is for
 * @param[in] content The new content of the value
 *                  
 * @retval 0 Success
 * @retval -1 Error
 */
BIONET_UTIL_API_DECL
int bionet_value_set_int16(bionet_value_t *value,
			   int16_t content);


/**
 * @brief Sets a value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] value The Value that the new content is for
 * @param[in] content The new content of the value
 *                  
 * @retval 0 Success
 * @retval -1 Error
 */
BIONET_UTIL_API_DECL
int bionet_value_set_uint32(bionet_value_t *value,
			    uint32_t content);


/**
 * @brief Sets a value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] value The Value that the new content is for
 * @param[in] content The new content of the value
 *                  
 * @retval 0 Success
 * @retval -1 Error
 */
BIONET_UTIL_API_DECL
int bionet_value_set_int32(bionet_value_t *value,
			   int32_t content);


/**
 * @brief Sets a value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] value The Value that the new content is for
 * @param[in] content The new content of the value
 *                  
 * @retval 0 Success
 * @retval -1 Error
 */
BIONET_UTIL_API_DECL
int bionet_value_set_float(bionet_value_t *value,
			   float content);


/**
 * @brief Sets a value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] value The Value that the new content is for
 * @param[in] content The new content of the value
 *                  
 * @retval 0 Success
 * @retval -1 Error
 */
BIONET_UTIL_API_DECL
int bionet_value_set_double(bionet_value_t *value,
			    double content);


/**
 * @brief Sets a value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] value The Value that the new content is for
 * @param[in] content The new content of the value
 *                  
 * @retval 0 Success
 * @retval -1 Error
 *
 * @note All passed-in strings are considered the property of the
 *       caller.  The caller is free to overwrite or free the
 *       strings on return from this function.
 */
BIONET_UTIL_API_DECL
int bionet_value_set_str(bionet_value_t *value,
			 const char * content);


/**
 * @brief Gets a value.  
 *
 * @param[in] value The Value
 * @param[out] content The content of the value
 *                  
 * @retval 0 Success
 * @retval -1 Failure
 * 
 * @note content is undefined if a failure occurred.
 */
BIONET_UTIL_API_DECL
int bionet_value_get_binary(const bionet_value_t *value,
			    int * content);


/**
 * @brief Gets a value.  
 *
 * @param[in] value The Value
 * @param[out] content The content of the value
 *                  
 * @retval 0 Success
 * @retval -1 Failure
 * 
 * @note content is undefined if a failure occurred.
 */
BIONET_UTIL_API_DECL
int bionet_value_get_uint8(const bionet_value_t *value,
			   uint8_t * content);


/**
 * @brief Gets a value.  
 *
 * @param[in] value The Value
 * @param[out] content The content of the value
 *                  
 * @retval 0 Success
 * @retval -1 Failure
 * 
 * @note content is undefined if a failure occurred.
 */
BIONET_UTIL_API_DECL
int bionet_value_get_int8(const bionet_value_t *value,
			  int8_t * content);


/**
 * @brief Gets a value.  
 *
 * @param[in] value The Value
 * @param[out] content The content of the value
 *                  
 * @retval 0 Success
 * @retval -1 Failure
 * 
 * @note content is undefined if a failure occurred.
 */
BIONET_UTIL_API_DECL
int bionet_value_get_uint16(const bionet_value_t *value,
			    uint16_t * content);


/**
 * @brief Gets a value.  
 *
 * @param[in] value The Value
 * @param[out] content The content of the value
 *                  
 * @retval 0 Success
 * @retval -1 Failure
 * 
 * @note content is undefined if a failure occurred.
 */
BIONET_UTIL_API_DECL
int bionet_value_get_int16(const bionet_value_t *value,
			   int16_t * content);


/**
 * @brief Gets a value.  
 *
 * @param[in] value The Value
 * @param[out] content The content of the value
 *                  
 * @retval 0 Success
 * @retval -1 Failure
 * 
 * @note content is undefined if a failure occurred.
 */
BIONET_UTIL_API_DECL
int bionet_value_get_uint32(const bionet_value_t *value,
			    uint32_t * content);


/**
 * @brief Gets a value.  
 *
 * @param[in] value The Value
 * @param[out] content The content of the value
 *                  
 * @retval 0 Success
 * @retval -1 Failure
 * 
 * @note content is undefined if a failure occurred.
 */
BIONET_UTIL_API_DECL
int bionet_value_get_int32(const bionet_value_t *value,
			   int32_t * content);


/**
 * @brief Gets a value.  
 *
 * @param[in] value The Value
 * @param[out] content The content of the value
 *                  
 * @retval 0 Success
 * @retval -1 Failure
 * 
 * @note content is undefined if a failure occurred.
 */
BIONET_UTIL_API_DECL
int bionet_value_get_float(const bionet_value_t *value,
			   float * content);


/**
 * @brief Gets a value.  
 *
 * @param[in] value The Value
 * @param[out] content The content of the value
 *                  
 * @retval 0 Success
 * @retval -1 Failure
 * 
 * @note content is undefined if a failure occurred.
 */
BIONET_UTIL_API_DECL
int bionet_value_get_double(const bionet_value_t *value,
			    double * content);


/**
 * @brief Gets a value.  
 *
 * @param[in] value The Value
 * @param[out] content The content of the value
 *                  
 * @retval 0 Success
 * @retval -1 Failure
 * 
 * @note content is undefined if a failure occurred.
 */
BIONET_UTIL_API_DECL
int bionet_value_get_str(const bionet_value_t *value,
			 char ** content);


/**
 * @brief Duplicates a value. 
 * 
 * @param[in] resource The resource the value is for
 * @param[in] value The Value
 * 
 * @retval NULL Failure
 * @return Pointer to a value
 *
 * @note The returned value is owned and must be free'd by the caller
 */
BIONET_UTIL_API_DECL
bionet_value_t * bionet_value_dup(const bionet_resource_t *resource, const bionet_value_t *value);

#ifdef __cplusplus
}
#endif

#endif /* __BIONET_VALUE_H */

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
