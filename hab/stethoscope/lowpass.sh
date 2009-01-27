#!/bin/sh

#
# Copyright (C) 2008-2009, Regents of the University of Colorado.
#


# 
# Reads a bionet-stream on stdin, runs it through a lowpass filter, and
# produces the filtered stream on stdout.
#

BIOSERVE_AUDIO_FORMAT_SOX="-t raw -r 9600 -c 1 -s -w"

sox $BIOSERVE_AUDIO_FORMAT_SOX - $BIOSERVE_AUDIO_FORMAT_SOX - lowpass 100 vol 16.0

