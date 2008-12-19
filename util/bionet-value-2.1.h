
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#ifndef __BIONET_VALUE_H
#define __BIONET_VALUE_H

/**
 * @file bionet-value.h
 * Functions for dealing with Bionet Values.
 */


#include <stdint.h>
#include <sys/time.h>

#include <glib.h>

#include "bionet-util.h"


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
 * @todo implement me
 */
bionet_value_t *bionet_value_new_binary(bionet_resource_t *resource,
					const int content);


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
 * @todo implement me
 */
bionet_value_t *bionet_value_new_uint8(bionet_resource_t *resource,
				       const uint8_t content);


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
 * @todo implement me
 */
bionet_value_t *bionet_value_new_int8(bionet_resource_t *resource,
				      const int8_t content);


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
 * @todo implement me
 */
bionet_value_t *bionet_value_new_uint16(bionet_resource_t *resource,
					const uint16_t content);


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
 * @todo implement me
 */
bionet_value_t *bionet_value_new_int16(bionet_resource_t *resource,
				       const int16_t content);


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
 * @todo implement me
 */
bionet_value_t *bionet_value_new_uint32(bionet_resource_t *resource,
					const uint32_t content);



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
 * @todo implement me
 */
bionet_value_t *bionet_value_new_int32(bionet_resource_t *resource,
				       const int32_t content);


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
 * @todo implement me
 */
bionet_value_t *bionet_value_new_float(bionet_resource_t *resource,
				       const float content);


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
 * @todo implement me
 */
bionet_value_t *bionet_value_new_double(bionet_resource_t *resource,
					const double content);


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
 * @todo implement me
 */
bionet_value_t *bionet_value_new_str(bionet_resource_t *resource,
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
 *
 * @todo implement me
 */
void bionet_value_free(bionet_value_t *value);


/**
 * @brief Renders a Value as an ASCII string.
 *
 * @param[in] value The value to create the string from
 *
 * @return A statically allocated string containing an ASCII
 *         representation of the Value
 * @return NULL Failure
 *
 * @note All passed-in strings are considered the property of the
 *       caller.  The caller is free to overwrite or free the
 *       strings on return from this function.
 *
 * @todo implement me
 */
const char *bionet_value_to_string(const bionet_value_t *value);


/**
 * @brief Sets a value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] value The Value that the new content is for
 * @param[in] content The new content of the value
 *                  
 * @return The new Value
 * @return NULL Error
 *
 * @todo implement me
 */
bionet_value_t *bionet_value_set_binary(bionet_value_t *value,
					const int content);


/**
 * @brief Sets a value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] value The Value that the new content is for
 * @param[in] content The new content of the value
 *                  
 * @return The new Value
 * @return NULL Error
 *
 * @todo implement me
 */
bionet_value_t *bionet_value_set_uint8(bionet_value_t *value,
				       const uint8_t content);


/**
 * @brief Sets a value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] value The Value that the new content is for
 * @param[in] content The new content of the value
 *                  
 * @return The new Value
 * @return NULL Error
 *
 * @todo implement me
 */
bionet_value_t *bionet_value_set_int8(bionet_value_t *value,
				      const int8_t content);


/**
 * @brief Sets a value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] value The Value that the new content is for
 * @param[in] content The new content of the value
 *                  
 * @return The new Value
 * @return NULL Error
 *
 * @todo implement me
 */
bionet_value_t *bionet_value_set_uint16(bionet_value_t *value,
					const uint16_t content);


/**
 * @brief Sets a value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] value The Value that the new content is for
 * @param[in] content The new content of the value
 *                  
 * @return The new Value
 * @return NULL Error
 *
 * @todo implement me
 */
bionet_value_t *bionet_value_set_int16(bionet_value_t *value,
				       const int16_t content);


/**
 * @brief Sets a value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] value The Value that the new content is for
 * @param[in] content The new content of the value
 *                  
 * @return The new Value
 * @return NULL Error
 *
 * @todo implement me
 */
bionet_value_t *bionet_value_set_uint32(bionet_value_t *value,
					const uint32_t content);


/**
 * @brief Sets a value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] value The Value that the new content is for
 * @param[in] content The new content of the value
 *                  
 * @return The new Value
 * @return NULL Error
 *
 * @todo implement me
 */
bionet_value_t *bionet_value_set_int32(bionet_value_t *value,
				       const int32_t content);


/**
 * @brief Sets a value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] value The Value that the new content is for
 * @param[in] content The new content of the value
 *                  
 * @return The new Value
 * @return NULL Error
 *
 * @todo implement me
 */
bionet_value_t *bionet_value_set_float(bionet_value_t *value,
				       const float content);


/**
 * @brief Sets a value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] value The Value that the new content is for
 * @param[in] content The new content of the value
 *                  
 * @return The new Value
 * @return NULL Error
 *
 * @todo implement me
 */
bionet_value_t *bionet_value_set_double(bionet_value_t *value,
					const double content);


/**
 * @brief Sets a value.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] value The Value that the new content is for
 * @param[in] content The new content of the value
 *                  
 * @return The new Value
 * @return NULL Error
 *
 * @note All passed-in strings are considered the property of the
 *       caller.  The caller is free to overwrite or free the
 *       strings on return from this function.
 *
 * @todo implement me
 */
bionet_value_t *bionet_value_set_str(bionet_value_t *value,
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
 *
 * @todo implement me
 */
int *bionet_value_get_binary(bionet_value_t *value,
			     const int * content);


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
 *
 * @todo implement me
 */
int *bionet_value_get_uint8(bionet_value_t *value,
			    const uint8_t * content);


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
 *
 * @todo implement me
 */
int *bionet_value_get_int8(bionet_value_t *value,
			   const int8_t * content);


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
 *
 * @todo implement me
 */
int *bionet_value_get_uint16(bionet_value_t *value,
			     const uint16_t * content);


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
 *
 * @todo implement me
 */
int *bionet_value_get_int16(bionet_value_t *value,
			    const int16_t * content);


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
 *
 * @todo implement me
 */
int *bionet_value_get_uint32(bionet_value_t *value,
			     const uint32_t * content);


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
 *
 * @todo implement me
 */
int *bionet_value_get_int32(bionet_value_t *value,
			    const int32_t * content);


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
 *
 * @todo implement me
 */
int *bionet_value_get_float(bionet_value_t *value,
			    const float * content);


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
 *
 * @todo implement me
 */
int *bionet_value_get_double(bionet_value_t *value,
			     const double * content);


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
 *
 * @todo implement me
 */
int *bionet_value_get_str(bionet_value_t *value,
			  const char ** content);


#endif /* __BIONET_VALUE_H */

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
