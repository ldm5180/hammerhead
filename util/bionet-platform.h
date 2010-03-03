// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#ifndef __BIONET_PLATFORM_H
#define __BIONET_PLATFORM_H

#ifdef __WIN32
#include <windows.h>

#ifdef __MINGW32__
#include <stdint.h>
#else

typedef UINT8 uint8_t;  
typedef UINT16 uint16_t;  
typedef UINT32 uint32_t;  
typedef UINT64 uint64_t;  
typedef INT8 int8_t;  
typedef INT16 int16_t;  
typedef INT32 int32_t;  
typedef INT64 int64_t;  
#endif

#else
#include <stdint.h>
#endif /* __WIN32 */

#endif /* __BIONET_PLATFORM_H */
