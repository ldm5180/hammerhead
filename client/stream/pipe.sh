#!/bin/sh
#
# Copyright (C) 2008-2009, Regents of the University of Colorado.
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

