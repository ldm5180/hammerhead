#!/usr/bin/python

from twisted.internet import reactor, interfaces, protocol
from twisted.protocols import basic
from twisted.web import server, resource
from twisted.web.static import File
from twisted.web.server import Site
from twisted.web.resource import Resource

import time

from twisted_bdm_client import *
from bdm_client import *
from bdmplot_callback_cacher import *
import bdmplot

import optparse

from prune_datapoints import *

def process_new_session_or_subscription(request):
    if ('resource' not in request.args) or ('timespan' not in request.args):
        return

    regex = None
    if 'regexp' in request.args:
        regex = request.args['regexp'][0]

    #create the session
    subscriptions.append( { 'filter' : request.args['resource'][0],
                            'timespan' : request.args['timespan'],
                            'last requested' : time.time(),
                            'regexp' : regex,
                            'new' : 0 } )

    # Convert the timespan into timevals and timestamps
    timespan_vals = timespan_to_timevals(request.args["timespan"][0])
    
    #subscribe to the resource name pattern requested in the HTTP request
    bdm_subscribe_datapoints_by_name(request.args['resource'][0], timespan_vals[0], timespan_vals[1])



class Datapoints(resource.Resource):
    isLeaf = True

    def render_GET(self, request):
        found = None
        # existing session
        for sub in subscriptions:
            if (sub['filter'] == request.args['resource'][0]) and (sub['timespan'] == request.args['timespan']):
                sub['last requested'] = time.time()
                found = sub
                break

        if (found == None): # new subscription!
            retval = process_new_session_or_subscription(request)
            for sub in subscriptions:
                if (sub['filter'] == request.args['resource'][0]) and (sub['timespan'] == request.args['timespan']):
                    found = sub
                    break
        
        (fname, format) = bdmplot.bdmplot(found, bionet_resources)
        request.setHeader('Content-Type', 'image/' + format)
        f = open(fname, 'rb')
        return f.read()

        #return retval

def main():
    # parse options 
    parser = optparse.OptionParser()
    parser.add_option("-p", "--port", dest="port",
                      help="Webserver port.", 
                      metavar="<port>",
                      default=8082)
    
    (options, args) = parser.parse_args()


    twisted_bdmclient = BdmClient()

    #register BDM callbacks
    pybdm_register_callback_new_hab(cb_new_hab, None)
    pybdm_register_callback_lost_hab(cb_lost_hab, None);
    pybdm_register_callback_new_node(cb_new_node, None);
    pybdm_register_callback_lost_node(cb_lost_node, None);
    pybdm_register_callback_datapoint(cb_datapoint, None);
    
    full = Datapoints()
    
    root = Resource()
    root.putChild('bdmplot', full)
    factory = Site(root)
    
    reactor.listenTCP(options.port, factory)
    reactor.addReader(twisted_bdmclient)
    
    reactor.run()


if __name__ == "__main__":
    main()
