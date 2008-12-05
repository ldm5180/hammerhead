#!/usr/bin/python

import time
from hab import *

test_hab = bionet_hab_new("test", "python")
bionet_fd = hab_connect(test_hab)

time.sleep(100)
