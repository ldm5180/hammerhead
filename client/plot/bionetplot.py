#!/usr/bin/python

# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.

# This library is free software. You can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as 
# published by the Free Software Foundation, version 2.1 of the License.
# This library is distributed in the hope that it will be useful, but 
# WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
# Lesser General Public License for more details. A copy of the GNU 
# Lesser General Public License v 2.1 can be found in the file named 
# "COPYING.LESSER".  You should have received a copy of the GNU Lesser 
# General Public License along with this library; if not, write to the 
# Free Software Foundation, Inc., 
# 51 Franklin Street, Fifth Floor, 
# Boston, MA 02110-1301 USA.
 
# You may contact the Automation Group at:
# bionet@bioserve.colorado.edu
 
# Dr. Kevin Gifford
# University of Colorado
# Engineering Center, ECAE 1B08
# Boulder, CO 80309
 
# Because BioNet was developed at a university, we ask that you provide
# attribution to the BioNet authors in any redistribution, modification, 
# work, or article based on this library.
 
# You may contribute modifications or suggestions to the University of
# Colorado for the purpose of discussing and improving this software.
# Before your modifications are incorporated into the master version 
# distributed by the University of Colorado, we must have a contributor
# license agreement on file from each contributor. If you wish to supply
# the University with your modifications, please join our mailing list.
# Instructions can be found on our website at 
# http://bioserve.colorado.edu/developers-corner.

from twisted.internet import reactor, interfaces, protocol
from twisted.protocols import basic
from twisted.web import server, resource
from twisted.web.static import File
from twisted.web.server import Site
from twisted.web.resource import Resource

from twisted_bionet_client import *
from bionet import *
from bionetplot_callback import *

import optparse, time


def process_new_session_or_subscription(sessions, session_id, request):
    if ('resource' not in request.args):
        return "<html>No subscription.</html>"

    if (session_id not in sessions):
        resource_list = [] #create the list of resources associated with this session

        #create the session
        sessions[session_id] = { 'resource'         : request.args['resource'],
                                 'bionet-resources' : resource_list,
                                 'last requested'   : None}

        #subscribe to all the resources requested in the HTTP request
        for r in sessions[session_id]['resource']:
            bionet_subscribe_datapoints_by_name(r)
    else:
        for r in request.args['resource']:
            sessions[session_id]['resource'].append(r)
            bionet_subscribe_datapoints_by_name(r)

    sessions[session_id]['last requested'] = time.time()

    return "{}"


class DataServer(resource.Resource):
    isLeaf = True

    def render_GET(self, request):
        session = request.getSession()

        if (session in sessions):
            sessions[session]['last requested'] = time.time()

        # existing session
        if (session in sessions) and (sessions[session]['resource'] == request.args['resource']):
            retval = "[ "
            #print "Old session found." #debugging
            for name in sessions[session]['bionet-resources']:
                retval += "{\n    ";
                #print "Looking at resource: %s" % name
                retval += "label: '%s',\n    data: [" % name
                u = bionet_resources[name]
                if (None != u) and ('sessions' in u) and (session in u['sessions']):
                    #print "Resource user_data:", u
                    for d in u['sessions'][session]:
                        retval += '[%s, %s], ' % (d[0], d[1])
                    u['sessions'][session] = []
                retval += "]\n"
                retval += "},\n"
                #print "u after loop: ", u
            #print retval
            retval += " ]"
            return "%s" % retval

        else: # new session or new subscription!
            retval = process_new_session_or_subscription(sessions, session, request)
            return retval



class Datapoints(resource.Resource):
    isLeaf = True

    def render_GET(self, request):
        session = request.getSession()

        if (session in sessions):
            sessions[session]['last requested'] = time.time()

        # existing session
        if (session in sessions) and (sessions[session]['resource'] == request.args['resource']):
            retval = "[ "
            #print "Old session found." #debugging
            for name in sessions[session]['bionet-resources']:
                retval += "{\n    ";
                #print "Looking at resource: %s" % name
                retval += "label: '%s',\n    data: [" % name
                u = bionet_resources[name]
                if (None != u) and ('datapoints' in u):
                    #print "Resource user_data:", u
                    u['sessions'][session] = []
                    for d in u['datapoints']:
                        retval += '[%s, %s], ' % (d[0], d[1])
                retval += "]\n"
                retval += "},\n"
                #print "u after loop: ", u
            #print retval
            retval += " ]"
            return "%s" % retval

        else: # new session!
            retval = process_new_session_or_subscription(sessions, session, request)
            return retval


def main():
    # parse options 
    parser = optparse.OptionParser()
    parser.add_option("-p", "--port", dest="port",
                      help="Webserver port.", 
                      metavar="<port>",
                      default=8080)
    
    (options, args) = parser.parse_args()


    twisted_client = Client()

    #register Bionet callbacks
    pybionet_register_callback_new_hab(cb_new_hab)
    pybionet_register_callback_lost_hab(cb_lost_hab);
    pybionet_register_callback_new_node(cb_new_node);
    pybionet_register_callback_lost_node(cb_lost_node);
    pybionet_register_callback_datapoint(cb_datapoint);
    
    data = DataServer()
    full = Datapoints()
    
    root = Resource()
    root.putChild('plot', File("plot.html"))
    root.putChild('flot', File("flot"))
    root.putChild('data', data)
    root.putChild('full', full)
    factory = Site(root)
    
    reactor.listenTCP(options.port, factory)
    reactor.addReader(twisted_client)
    
    reactor.run()


if __name__ == "__main__":
    main()
