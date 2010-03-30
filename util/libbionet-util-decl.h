
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef _LIBBIONET_UTIL_DECL_H
#define _LIBBIONET_UTIL_DECL_H

#if defined(__WIN32)
#  if defined(BUILDING_LIBBIONET_UTIL)
#    ifdef DLL_EXPORT
#      define BIONET_UTIL_API_DECL __declspec(dllexport)
#    else
#      define BIONET_UTIL_API_DECL
#    endif
#  else
#    define BIONET_UTIL_API_DECL __declspec(dllimport)
#  endif
#else
#  define BIONET_UTIL_API_DECL extern
#endif



#endif /*  _LIBBIONET_UTIL_DECL_H */
