#!/usr/bin/python
# vim: sw=4 sta expandtab
#
#

from twisted.internet import reactor
from twisted.internet import defer
from twisted.protocols import basic

from twisted_bionet_client import *
from bdm_client import *

def dp_callback(this_mon, datapoint):
    value = bionet_datapoint_get_value(datapoint);
    resource = bionet_value_get_resource(value);

    resource_name = bionet_resource_get_name(resource)
    val_str = bionet_value_to_str(value)

    if (resource_name == this_mon.recorded_resource):
        this_mon.stats['recorded_events'] = int(val_str)
    elif (resource_name == this_mon.sync_received_resource):
        this_mon.stats['sync_received_events'] = int(val_str)
    elif (resource_name == this_mon.syncd_resource):
        this_mon.stats['syncd_events'] = int(val_str)
    elif (resource_name == this_mon.sent_syncs_resource):
        this_mon.stats['sent_syncs'] = int(val_str)
    elif (resource_name == this_mon.sent_acks_resource):
        this_mon.stats['sent_acks'] = int(val_str)
    elif (resource_name == this_mon.recvd_acks_resource):
        this_mon.stats['recvd_acks'] = int(val_str)

    if ( this_mon.stats['recorded_events'] > 0 \
        and this_mon.stats['recvd_acks'] > 0 \
        and this_mon.stats['recorded_events'] == this_mon.stats['sync_received_events'] \
        and this_mon.stats['recvd_acks'] == this_mon.stats['sent_acks'] \
        and this_mon.stats['recvd_acks'] == this_mon.stats['sent_syncs'] \
        and this_mon.stats['recorded_events'] == this_mon.stats['syncd_events']):
        this_mon.notifyComplete(True)



class SyncMonitor(basic.LineReceiver):

    def __init__(self, bdm_id_send, bdm_id_recv, timeout=60):
        self.waiting = True;

        tc = Client()
        reactor.addReader(tc)



        self.timeout = timeout

        this_mon = self

        # We need to keep a ref to this_cb...
        self.this_cb = lambda x: dp_callback(self,x)
        f = pybionet_register_callback_datapoint(self.this_cb)
        if(f == None):
            print "Failed to register callback!!!"


        self.recorded_resource = "Bionet-Data-Manager.%s.Statistics:Recorded-Bionet-Events" % bdm_id_send
        self.syncd_resource = "Bionet-Data-Manager.%s.Statistics:Sync-Sent-Events" % bdm_id_send
        self.sync_received_resource = "Bionet-Data-Manager.%s.Statistics:Sync-Received-Events" % bdm_id_recv
        self.sent_syncs_resource = "Bionet-Data-Manager.%s.Statistics:Sync-Messages-Sent" % bdm_id_send
        self.sent_acks_resource = "Bionet-Data-Manager.%s.Statistics:Sync-Acks-Sent" % bdm_id_recv
        self.recvd_acks_resource = "Bionet-Data-Manager.%s.Statistics:Sync-Acks-Received" % bdm_id_send

        self.stats = {
                'complete': False,
                'recorded_events': 0,
                'syncd_events': 0,
                'sync_received_events': 0,
                'sent_syncs': 0,
                'sent_acks': 0,
                'recvd_acks': 0,
                }

        bionet_subscribe_datapoints_by_name(self.recorded_resource)
        bionet_subscribe_datapoints_by_name(self.syncd_resource)
        bionet_subscribe_datapoints_by_name(self.sync_received_resource)
        bionet_subscribe_datapoints_by_name(self.sent_syncs_resource)
        bionet_subscribe_datapoints_by_name(self.sent_acks_resource)
        bionet_subscribe_datapoints_by_name(self.recvd_acks_resource)



    def notifyComplete(self,done):
        if(done):
            print "Bdm Sync Complete"
            self.stats['complete'] = True
        else:
            print "Bdm Sync Timeout"
            self.stats['complete'] = False

        self.waiting = False
        if self.d:
            self.d.callback(self.stats)
            self.d = None

    def getStatusDeferred(self):
        self.d = defer.Deferred()
        reactor.callLater(self.timeout, self.notifyComplete,False)

        return self.d



import sys
import optparse
import time

RC = 1
tStart = time.time()

def __doReport(stats):
    global RC
    global tStart
    if stats['complete']:
        print "Complete in %d seconds" % (time.time() - tStart )
        RC=0
    else:
        print "Timeout in %d seconds" % (time.time() - tStart )
        RC=2
    
    print "  Recorded Events:      %(recorded_events)d" % stats
    print "  Syncd Events:         %(syncd_events)d" % stats
    print "  Sync Received Events: %(sync_received_events)d" % stats
    print "  Sync Acks Sent:       %(sent_syncs)d" % stats
    print "  Sync Acks Received:   %(recvd_acks)d" % stats
    print "  Sync Acks Sent:       %(sent_acks)d" % stats
    
    reactor.stop()



if __name__ == "__main__":
    usage = """usage: %prog [options]

Wait for sync to be complete. Exits success if complete, or error if timeout"""

    parser = optparse.OptionParser(usage=usage)

    parser.add_option("--send-id",
        dest="sendid",
        type="string",
        help="BDM ID of the sync sending BDM's")

    parser.add_option("--recv-id",
        dest="recvid",
        type="string",
        help="BDM ID of the sync receiving BDM's")

    parser.add_option("--timeout",
        dest="timeout",
        type="int",
        default=120,
        help="Timeout in seconds to wait for sync to complete")


    (opts,args) = parser.parse_args()

    m = SyncMonitor(opts.sendid, opts.recvid, timeout=opts.timeout)

    
    d = m.getStatusDeferred()
    d.addCallback(__doReport)

    reactor.callLater(int(opts.timeout)+10, reactor.stop)
    reactor.run()
    sys.exit(RC)
