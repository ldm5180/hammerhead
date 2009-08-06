#!/usr/bin/python

import BadHab
import socket
import signal
import time


hab = None

def exit_handler(signum, frame):
    if (hab != None ):
        hab.unpublish()
    print '-- signal %d caught. Exiting --' % signum
    exit(0)



hab = BadHab.BadHab()

host = socket.gethostname()
name = 'bad-hab.' + host

signal.signal(signal.SIGTERM, exit_handler)
signal.signal(signal.SIGINT, exit_handler)

hab.setFailModes( connectTimeout = True )
hab.publish(name)

time.sleep(30)

hab.unpublish()
