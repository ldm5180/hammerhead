
# Copyright (c) 2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

# this puts it in /usr/lib/python2.5/site-packages
python_PYTHON = twisted_bionet.py

bin_SCRIPTS = mon-hab

nobase_sysconf_DATA =           \
    mon-hab.d/config            \
    mon-hab.d/nodes.d/mem-free  \
    mon-hab.d/nodes.d/date      \
    mon-hab.d/nodes.d/df        \
    mon-hab.d/nodes.d/load

