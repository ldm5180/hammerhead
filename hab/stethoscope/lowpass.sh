#!/bin/sh
#
# Copyright (C) 2004-2008, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G and NNC06CB40C.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of GNU General Public License version 2, as
# published by the Free Software Foundation.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
# 


# 
# Reads a bionet-stream on stdin, runs it through a lowpass filter, and
# produces the filtered stream on stdout.
#

BIOSERVE_AUDIO_FORMAT_SOX="-t raw -r 9600 -c 1 -s -w"

sox $BIOSERVE_AUDIO_FORMAT_SOX - $BIOSERVE_AUDIO_FORMAT_SOX - lowpass 100 vol 16.0

