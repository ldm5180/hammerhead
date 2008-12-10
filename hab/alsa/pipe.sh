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


#BSTREAM="../../client/stream/bionet-stream"
BSTREAM="bionet-stream"


NAG="bionet-base"

if [ "$1" = "--nag" ]; then
    shift
    NAG="$1"
    shift
fi


SRC_STREAM=$1
DEST_STREAM=$2


$BSTREAM --nag $NAG $SRC_STREAM | $BSTREAM --nag $NAG $DEST_STREAM

