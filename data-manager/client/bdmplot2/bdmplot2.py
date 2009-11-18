#!/usr/bin/python

from twisted.internet import reactor, interfaces, protocol
from twisted.protocols import basic
from twisted.web import server, resource
from twisted.web.static import File
from twisted.web.server import Site
from twisted.web.resource import Resource

from twisted_bionet_client import *
from bdm_client import *
from bdmplot_callback import *

import optparse

class DataServer(resource.Resource):
    isLeaf = True

    def render_GET(self, request):
        session = request.getSession()

        # existing session
        if (session in sessions) and (sessions[session]['resource'] == request.args['resource']) and (sessions[session]['timespan'] == request.args['timespan']):
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

        else: # new session!
            if ('resource' not in request.args) or ('timespan' not in request.args):
                return "<html>No subscription.</html>"

            resource_list = [] #create the list of resources associated with this session

            #create the session
            sessions[session] = { 'resource' : request.args['resource'],
                                  'timespan' : request.args['timespan'],
                                  'bionet-resources' : resource_list }

            #subscribe to all the resources requested in the HTTP request
            for r in sessions[session]['resource']:
                #print "Subscribing to %(resource)s" % { 'resource' : r }
                bionet_subscribe_datapoints_by_name(r)

            return "{}"


class Datapoints(resource.Resource):
    isLeaf = True

    def render_GET(self, request):
        session = request.getSession()

        # existing session
        if (session in sessions) and (sessions[session]['resource'] == request.args['resource']) and (sessions[session]['timespan'] == request.args['timespan']):
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
            if ('resource' not in request.args) or ('timespan' not in request.args):
                return "<html>No subscription.</html>"

            resource_list = [] #create the list of resources associated with this session

            #create the session
            sessions[session] = { 'resource' : request.args['resource'],
                                  'timespan' : request.args['timespan'],
                                  'bionet-resources' : resource_list }

            #subscribe to all the resources requested in the HTTP request
            for r in sessions[session]['resource']:
                #print "Subscribing to %(resource)s" % { 'resource' : r }
                bionet_subscribe_datapoints_by_name(r)

            return "{}"


def main():
    # parse options 
    parser = optparse.OptionParser()
    parser.add_option("-p", "--port", dest="port",
                      help="Webserver port.", 
                      metavar="<port>",
                      default=8081)
    
    (options, args) = parser.parse_args()


    twisted_bdmclient = BdmClient()

    #register Bionet callbacks
    pybionet_register_callback_new_hab(cb_new_hab)
    pybionet_register_callback_lost_hab(cb_lost_hab);
    pybionet_register_callback_new_node(cb_new_node);
    pybionet_register_callback_lost_node(cb_lost_node);
    pybionet_register_callback_datapoint(cb_datapoint);
    
    data = DataServer()
    full = Datapoints()
    
    root = Resource()
    root.putChild('bdmplot', File("plot.html"))
    root.putChild('flot', File("flot"))
    root.putChild('data', data)
    root.putChild('full', full)
    factory = Site(root)
    
    reactor.listenTCP(options.port, factory)
    reactor.addReader(twisted_client)
    
    reactor.run()


if __name__ == "__main__":
    main()
