
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef CHECK_COMMON_H
#define CHECK_COMMON_H

#define SIZEOF(x) (sizeof(x)/sizeof(x[0]))

typedef void (*suite_func_t)(Suite *suite);

typedef struct
{
    const char *name;
    suite_func_t fp;
} suite_t;

int run_suite(suite_t suite);


#endif /* CHECK_COMMON_H */

// vim: ts=8 sw=4 sta
// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// indent-tabs-mode: nil
// End:

