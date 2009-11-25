#!/usr/bin/python

from twisted.internet import stdio, reactor
from twisted.protocols import basic

from twisted_bdm_client import *
from bdm_client import *
from bdm_normal_output import BDMNormalOutput

import optparse

class BdmWatcher(basic.LineReceiver):
    delimiter = '\n'
    
    def __init__(self, logfile=None ):
        self.twisted_bdmclient = BdmClient()

        output = BDMNormalOutput(logfile)
        output.handle_callbacks()

        reactor.addReader(self.twisted_bdmclient)

    # Inspired by the stdiodemo.py example
    def lineReceived(self, line):
        # Ignore blank lines
        if not line: return

        # Parse the command
        commandParts = line.split()
        command = commandParts[0].lower()
        args = commandParts[1:]

        # Dispatch the command to the appropriate method.  Note that all you
        # need to do to implement a new command is add another do_* method.
        try:
            method = getattr(self, 'do_' + command)
        except AttributeError, e:
            self.sendLine('Error: no such command.')
        else:
            try:
                method(*args)
            except Exception, e:
                self.sendLine('Error: ' + str(e))

    def do_help(self, command=None):
        """help [command]: List commands, or show help on the given command"""
        if command:
            self.sendLine(getattr(self, 'do_' + command).__doc__)
        else:
            commands = [cmd[3:] for cmd in dir(self) if cmd.startswith('do_')]
            self.sendLine("Valid commands: " +" ".join(commands))
        
    def do_subscribe_datapoints(self, pattern):
        """subscribe to a resource name pattern"""
        self.sendLine('Subscribing to "' + pattern + '"')
        bdm_subscribe_datapoints_by_name(pattern, None, None)

    def do_quit(self):
        """quit: Quit this session"""
        self.transport.loseConnection()

    def connectionLost(self, reason):
        # stop the reactor, only because this is meant to be run in Stdio.
        self.sendLine('Goodbye.')
        reactor.stop()
        




if __name__ == "__main__":
    # parse options 
    parser = optparse.OptionParser()
    parser.add_option("-l", "--log", dest="logfile",
                      help="Logfile", 
                      metavar="<log>",
                      default='/dev/stdout')
    
    (options, args) = parser.parse_args()
    stdio.StandardIO(BdmWatcher(options.logfile))
    reactor.run()


#
# vim: sw=4 sta expandtab
#
