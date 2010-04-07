#!/bin/sh
#
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.
#


# 
# Reads a bionet-stream on stdin, runs it through a lowpass filter, and
# produces the filtered stream on stdout.
#

BIOSERVE_AUDIO_FORMAT_SOX="-t raw -r 9600 -c 1 -s -w"

sox $BIOSERVE_AUDIO_FORMAT_SOX - $BIOSERVE_AUDIO_FORMAT_SOX - lowpass 100 vol 16.0

