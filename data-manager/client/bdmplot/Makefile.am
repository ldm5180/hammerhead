
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

python_PYTHON = bdmplot_lib.py \
	bdmplot_callback_cacher.py \
	prune_datapoints.py \
	bdmplot_timespan.py \
	bdmplot_daemon.py \
	bdmplot_datapoints_to_dict.py

bin_SCRIPTS = bdmplot

nobase_dist_sysconf_DATA = init.d/bdmplot

nobase_dist_data_DATA = cgi-bin/bdmplot
