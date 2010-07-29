
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef __BIONET_EPSILON_H
#define __BIONET_EPSILON_H

/**
 * @file bionet-epsilon.h
 * Functions for dealing with Bionet Epsilons.
 */


#include "bionet-platform.h"

#include "libbionet-util-decl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Allocates and initializes a new epsilon.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] content The content
 *                  
 * @return The new Epsilon
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_epsilon_t *bionet_epsilon_new_binary(int content);


/**
 * @brief Allocates and initializes a new epsilon.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] content The content
 *                  
 * @return The new Epsilon
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_epsilon_t *bionet_epsilon_new_uint8(uint8_t content);


/**
 * @brief Allocates and initializes a new epsilon.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] content The content
 *                  
 * @return The new Epsilon
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_epsilon_t *bionet_epsilon_new_int8(int8_t content);


/**
 * @brief Allocates and initializes a new epsilon.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] content The content
 *                  
 * @return The new Epsilon
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_epsilon_t *bionet_epsilon_new_uint16(uint16_t content);


/**
 * @brief Allocates and initializes a new epsilon.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] content The content
 *                  
 * @return The new Epsilon
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_epsilon_t *bionet_epsilon_new_int16(int16_t content);


/**
 * @brief Allocates and initializes a new epsilon.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] content The content
 *                  
 * @return The new Epsilon
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_epsilon_t *bionet_epsilon_new_uint32(uint32_t content);



/**
 * @brief Allocates and initializes a new epsilon.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] content The content
 *                  
 * @return The new Epsilon
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_epsilon_t *bionet_epsilon_new_int32(int32_t content);


/**
 * @brief Allocates and initializes a new epsilon.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] content The content
 *                  
 * @return The new Epsilon
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_epsilon_t *bionet_epsilon_new_float(float content);


/**
 * @brief Allocates and initializes a new epsilon.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] content The content
 *                  
 * @return The new Epsilon
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_epsilon_t *bionet_epsilon_new_double(double content);


/**
 * @brief Allocates and initializes a new epsilon.  
 *
 * Does NOT add it to the Resource's list of datapoints.
 *
 * @param[in] content The content
 *                  
 * @return The new Epsilon
 * @return NULL Error
 *
 * @note The memory in the content pointer is copied, so the memory
 * pointed to by content is still owned by the caller.
 */
BIONET_UTIL_API_DECL
bionet_epsilon_t *bionet_epsilon_new_str(int content);


/**
 * @brief Frees a Epsilon
 *
 * Epsilon was created with one of the following:
 * bionet_epsilon_new_binary() or bionet_epsilon_new_str()
 * bionet_epsilon_new_uint8() or bionet_epsilon_new_int8() 
 * bionet_epsilon_new_uint16() or bionet_epsilon_new_int16() 
 * bionet_epsilon_new_uint32() or bionet_epsilon_new_int32()
 * bionet_epsilon_new_float() or bionet_epsilon_new_double() 
 *
 * @param[in] epsilon The Epsilon to free.
 */
BIONET_UTIL_API_DECL
void bionet_epsilon_free(bionet_epsilon_t *epsilon);


/**
 * @brief Renders a Epsilon as an ASCII string.
 *
 * @param[in] epsilon The epsilon to create the string from
 *
 * @return A dynamically allocated string containing an ASCII
 *         representation of the Epsilon
 * @return NULL Failure
 *
 * @note The returned string becomes the property (and responsibility) of
 *       the caller.  The caller must free the returned string or leak
 *       memory.
 */
BIONET_UTIL_API_DECL
char *bionet_epsilon_to_str(const bionet_epsilon_t *epsilon,
			  bionet_resource_data_type_t data_type);


/**
 * @brief Gets a epsilon.  
 *
 * @param[in] epsilon The Epsilon
 * @param[out] content The content of the epsilon
 *                  
 * @retval 0 Success
 * @retval -1 Failure
 * 
 * @note content is undefined if a failure occurred.
 */
BIONET_UTIL_API_DECL
int bionet_epsilon_get_binary(const bionet_epsilon_t *epsilon,
			    int * content);


/**
 * @brief Gets a epsilon.  
 *
 * @param[in] epsilon The Epsilon
 * @param[out] content The content of the epsilon
 *                  
 * @retval 0 Success
 * @retval -1 Failure
 * 
 * @note content is undefined if a failure occurred.
 */
BIONET_UTIL_API_DECL
int bionet_epsilon_get_uint8(const bionet_epsilon_t *epsilon,
			   uint8_t * content);


/**
 * @brief Gets a epsilon.  
 *
 * @param[in] epsilon The Epsilon
 * @param[out] content The content of the epsilon
 *                  
 * @retval 0 Success
 * @retval -1 Failure
 * 
 * @note content is undefined if a failure occurred.
 */
BIONET_UTIL_API_DECL
int bionet_epsilon_get_int8(const bionet_epsilon_t *epsilon,
			  int8_t * content);


/**
 * @brief Gets a epsilon.  
 *
 * @param[in] epsilon The Epsilon
 * @param[out] content The content of the epsilon
 *                  
 * @retval 0 Success
 * @retval -1 Failure
 * 
 * @note content is undefined if a failure occurred.
 */
BIONET_UTIL_API_DECL
int bionet_epsilon_get_uint16(const bionet_epsilon_t *epsilon,
			    uint16_t * content);


/**
 * @brief Gets a epsilon.  
 *
 * @param[in] epsilon The Epsilon
 * @param[out] content The content of the epsilon
 *                  
 * @retval 0 Success
 * @retval -1 Failure
 * 
 * @note content is undefined if a failure occurred.
 */
BIONET_UTIL_API_DECL
int bionet_epsilon_get_int16(const bionet_epsilon_t *epsilon,
			   int16_t * content);


/**
 * @brief Gets a epsilon.  
 *
 * @param[in] epsilon The Epsilon
 * @param[out] content The content of the epsilon
 *                  
 * @retval 0 Success
 * @retval -1 Failure
 * 
 * @note content is undefined if a failure occurred.
 */
BIONET_UTIL_API_DECL
int bionet_epsilon_get_uint32(const bionet_epsilon_t *epsilon,
			    uint32_t * content);


/**
 * @brief Gets a epsilon.  
 *
 * @param[in] epsilon The Epsilon
 * @param[out] content The content of the epsilon
 *                  
 * @retval 0 Success
 * @retval -1 Failure
 * 
 * @note content is undefined if a failure occurred.
 */
BIONET_UTIL_API_DECL
int bionet_epsilon_get_int32(const bionet_epsilon_t *epsilon,
			   int32_t * content);


/**
 * @brief Gets a epsilon.  
 *
 * @param[in] epsilon The Epsilon
 * @param[out] content The content of the epsilon
 *                  
 * @retval 0 Success
 * @retval -1 Failure
 * 
 * @note content is undefined if a failure occurred.
 */
BIONET_UTIL_API_DECL
int bionet_epsilon_get_float(const bionet_epsilon_t *epsilon,
			   float * content);


/**
 * @brief Gets a epsilon.  
 *
 * @param[in] epsilon The Epsilon
 * @param[out] content The content of the epsilon
 *                  
 * @retval 0 Success
 * @retval -1 Failure
 * 
 * @note content is undefined if a failure occurred.
 */
BIONET_UTIL_API_DECL
int bionet_epsilon_get_double(const bionet_epsilon_t *epsilon,
			    double * content);


/**
 * @brief Gets a epsilon.  
 *
 * @param[in] epsilon The Epsilon
 * @param[out] content The content of the epsilon
 *                  
 * @retval 0 Success
 * @retval -1 Failure
 * 
 * @note content is undefined if a failure occurred.
 */
BIONET_UTIL_API_DECL
int bionet_epsilon_get_str(const bionet_epsilon_t *epsilon,
			 int * content);



#ifdef __cplusplus
}
#endif

#endif /* __BIONET_EPSILON_H */

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
