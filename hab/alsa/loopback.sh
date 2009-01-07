#!/bin/sh
#
# Copyright (C) 2008-2009, Regents of the University of Colorado.
# 


BSTREAM="../../client/stream/bionet-stream"


NAG="bionet-base"

if [ "$1" = "--nag" ]; then
    shift
    NAG="$1"
    shift
fi


NODE=$1


$BSTREAM --nag $NAG $NODE:Microphone | $BSTREAM --nag $NAG $NODE:Speaker

