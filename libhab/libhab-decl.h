
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef _LIBHAB_DECL_H
#define _LIBHAB_DECL_H

#ifdef __WIN32
#  if defined(BUILDING_LIBHAB)
#    ifdef DLL_EXPORT
#      define LIBHAB_API_DECL __declspec(dllexport)
#    else
#      define LIBHAB_API_DECL
#    endif
#  else
#    define LIBHAB_API_DECL __declspec(dllimport)
#  endif
#else
#  define LIBHAB_API_DECL extern
#endif



#endif /*  _LIBHAB_DECL_H */
