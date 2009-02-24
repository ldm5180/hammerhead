#!/bin/sh
#
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.
#


cd platforms/petalinux/user-apps
./easy_build.sh
cd ../../..

./configure-mdacs.sh
make
