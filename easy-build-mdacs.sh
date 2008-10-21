#!/bin/sh

cd platforms/petalinux/user-apps
./easy_build.sh
cd ../../..

./configure-mdacs.sh
make
