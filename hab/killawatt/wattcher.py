#!/usr/bin/env python
import serial, time, datetime, sys
from xbee import xbee
import sensorhistory
import logging
import optparse
from hab import *

parser = optparse.OptionParser()
parser.add_option("-i", "--interval", dest="interval", default="30",help="Choose the number of seconds desired between data collections (defaults to 30 seconds)", metavar = "INT")
parser.add_option("-d", "--debug", dest="debug", default=None,help="-d will read out all debug messages", metavar = "DEBUG")
parser.add_option("-s", "--ser_port", dest="ser_port", default = "/dev/ttyUSB0", help="Choose the serial port (default /dev/ttyUSB0)", metavar = "SER")

(options, args) = parser.parse_args()

logger = logging.getLogger("Bionet Kill-A-Watt HAB")
logger.setLevel(logging.WARNING)
ch = logging.StreamHandler()
ch.setLevel(logging.WARNING)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)
logger.addHandler(ch)

BAUDRATE = 9600      # the baud rate we talk to the xbee
CURRENTSENSE = 4       # which XBee ADC has current draw data
VOLTSENSE = 0          # which XBee ADC has mains voltage data
MAINSVPP = 170 * 2     # +-170V is what 120Vrms ends up being (= 120*2sqrt(2))
vrefcalibration = [492]
CURRENTNORM = 15.5  # conversion to amperes from ADC
NUMWATTDATASAMPLES = 1800 # how many samples to watch in the plot window, 1 hr @ 2s samples


# open up the FTDI serial port to get data transmitted to xbee
ser = serial.Serial(options.ser_port, BAUDRATE)
ser.open()

#connect to bionet
hab = bionet_hab_new("Kill-A-Watt", None)
bionet_fd = hab_connect(hab)
if (0 > bionet_fd):
    logger.critical("Problem connection to Bionet, exiting\n")
    exit(1)
            
sensorhistories = sensorhistory.SensorHistories()
print sensorhistories

addresses = {}
intervals = {}
cal_ampdata = {}

def add_node(hab, xb_address_16):
    #check if there is a new node    
    node = bionet_hab_get_node_by_id(hab, str(xb_address_16))
    if (node == None):
        #create and report node
        node = bionet_node_new(hab, str(xb_address_16))
        if (node == None):
            logger.critical("Critical - Error getting new node")
            sys.exit(1)
        if (bionet_hab_add_node(hab, node)):
            logger.critical("Critical - Error adding node to hab")
            sys.exit(1)
        resource = bionet_resource_new(node, BIONET_RESOURCE_DATA_TYPE_FLOAT, BIONET_RESOURCE_FLAVOR_SENSOR, "Amps")
        if (resource == None):
            logger.critical("Critical - Error creating Resource - Amps")
            sys.exit(1)
        if (bionet_node_add_resource(node, resource)):
            logger.critical("Error adding resource to node")
            sys.exit(1)
        resource = bionet_resource_new(node, BIONET_RESOURCE_DATA_TYPE_FLOAT, BIONET_RESOURCE_FLAVOR_SENSOR, "Watts")
        if (resource == None):
            logger.critical("Critical - Error creating Resource - Watts")
            sys.exit(1)
        if (bionet_node_add_resource(node, resource)):
            logger.critical("Error adding resource to node")
            sys.exit(1)
        resource = bionet_resource_new(node, BIONET_RESOURCE_DATA_TYPE_FLOAT, BIONET_RESOURCE_FLAVOR_SENSOR, "KW-Hours")
        if (resource == None):
            logger.critical("Critical - Error creating Resource - KW-Hours")
            sys.exit(1)
        if (bionet_node_add_resource(node, resource)):
            logger.critical("Error adding resource to node")
            sys.exit(1)
        resource = bionet_resource_new(node, BIONET_RESOURCE_DATA_TYPE_FLOAT, BIONET_RESOURCE_FLAVOR_SENSOR, "Total-KW-Hours")
        if (resource == None):
            logger.critical("Critical - Error creating Resource - Total-KW-Hours")
            sys.exit(1)
        if (bionet_node_add_resource(node, resource)):
            logger.critical("Error adding resource to node")
            sys.exit(1)
        if (hab_report_new_node(node)):
            logger.critical("Critical - Error reporting node")
    return node


def calculations(voltagedata, ampdata, xb_analog_samples, xb_address_16, sensorhistory):
    global newamps, newwatts, cal_ampdata, addresses

    if (options.debug != None):
        logger.debug("Debug - ampdata: "+str(ampdata))
        logger.debug("Debug - voltdata: "+str(voltagedata))

    for i in range(len(voltagedata)):
        voltagedata[i] = xb_analog_samples[i+1][VOLTSENSE]
        ampdata[i] = xb_analog_samples[i+1][CURRENTSENSE]
    
    count_ampdata = 0
    cal_ampdata[xb_address_16] = 0

    for i in range(len(ampdata)):
        if(ampdata[i] > 400 and ampdata[i] < 600):
            cal_ampdata[xb_address_16] += ampdata[i]
            count_ampdata = count_ampdata + 1
    if count_ampdata != 0:
        cal_ampdata[xb_address_16] = cal_ampdata[xb_address_16]/count_ampdata

    # get max and min voltage and normalize the curve to '0'
    # to make the graph 'AC coupled' / signed
    min_v = 1024     # XBee ADC is 10 bits, so max value is 1023
    max_v = 0
    for i in range(len(voltagedata)):
        if (min_v > voltagedata[i]):
            min_v = voltagedata[i]
        if (max_v < voltagedata[i]):
            max_v = voltagedata[i]

    # figure out the 'average' of the max and min readings
    avgv = (max_v + min_v) / 2
    # also calculate the peak to peak measurements
    vpp =  max_v-min_v

    for i in range(len(voltagedata)):
    #remove 'dc bias', which we call the average read
        voltagedata[i] -= avgv
        # We know that the mains voltage is 120Vrms = +-170Vpp
        if vpp != 0:
	    voltagedata[i] = (voltagedata[i] * MAINSVPP) / vpp
	else:
	    voltagedata[i] = 0
    # normalize current readings to amperes
    for i in range(len(ampdata)):
    # VREF is the hardcoded 'DC bias' value, its
    # about 492 but would be nice if we could somehow get this data once in a while maybe using xbeeAPI
        if cal_ampdata[xb_address_16]:
            ampdata[i] -= cal_ampdata[xb_address_16]
        else:
            ampdata[i] -= vrefcalibration[0]
        # the CURRENTNORM is our normalizing constant
        # that converts the ADC reading to Amperes
        ampdata[i] /= CURRENTNORM

    #calculate instant watts by multiplying V*I for each sample
    wattdata = [0] * len(voltagedata)
    for i in range(len(wattdata)):
        wattdata[i] = voltagedata[i] * ampdata[i]

    new_calculations = addresses[xb_address_16]

    # sum up the current drawn over one 1/60hz cycle
    new_calculations["avgamp"] = 0
    # 16.6 samples per second, one cycle = ~17 samples
    num_samples = 17
    for i in range(num_samples):
        new_calculations["avgamp"] += abs(ampdata[i])
    new_calculations["avgamp"] /= num_samples

    # sum up power drawn over one 1/60hz cycle
    new_calculations["avgwatt"] = 0
    for i in range(num_samples):
        new_calculations["avgwatt"] += abs(wattdata[i])
    new_calculations["avgwatt"] /= num_samples
    # Print out our most recent measurements
    logger.info(str(xb_address_16)+"Current draw, in amperes: "+str(new_calculations["avgamp"]))
    logger.info("Watt draw, in VA: "+str(new_calculations["avgwatt"]))
    if (new_calculations["avgamp"] > 13):
	logger.critical("Reporting bad data")
    
    # add up the delta-watthr used since last reading
    # Figure out how many watt hours were used since last reading
    elapsedseconds = time.time() - sensorhistory.lasttime
    new_calculations["WattHr"] = (new_calculations["avgwatt"] * elapsedseconds) / (60.0 * 60.0)  # 60 seconds in 60 minutes = 1 hr
    sensorhistory.lasttime = time.time()
    logger.info("Wh used in last "+str(elapsedseconds)+" seconds: "+str(new_calculations["WattHr"]))
    sensorhistory.addwatthr(new_calculations["WattHr"])
    
    new_calculations["TotalWattHr"] += new_calculations["WattHr"]

    new_calculations["newamps"] += new_calculations["avgamp"]
    new_calculations["newwatts"] += new_calculations["avgwatt"]
    
    new_calculations["NumberSamples"] += 1
    return new_calculations

# the 'main loop' runs once a second or so
def update_graph(idleevent):
    global avgwattdataidx, sensorhistories, DEBUG, intervals, addresses
    packet = xbee.find_packet(ser)
    hab_read()
    if not packet:
        return
    xb = xbee(packet)
    #print "Got a packet: %s, makes an xb: %s, address: %d, xb.analog_samples: %s" % (packet.encode("hex"), repr(xb), xb.address_16, xb.analog_samples)
    sensorhistory = sensorhistories.findwithoutcreating(xb.address_16)

    # If all the analog samples are zero, consider the KaW dead
    isDeadByCurrent = True
    isDeadByVoltage = True
    for i in range(len(xb.analog_samples)):
        if xb.analog_samples[i][CURRENTSENSE] != 0:
            isDeadByCurrent = False
	if xb.analog_samples[i][VOLTSENSE] != 0:
            isDeadByVoltage = False
    isDead = isDeadByCurrent or isDeadByVoltage

    if isDead:
	if sensorhistory == None:
		return
#	print "Considering Kill-a-Watt %d to be dead" % xb.address_16
	node = bionet_hab_remove_node_by_id(hab, str(xb.address_16))
	hab_report_lost_node(str(xb.address_16))
	bionet_node_free(node)
        intervals[xb.address_16] = 0
	del addresses[xb.address_16]
	sensorhistories.remove(xb.address_16)
	return

    # If it isn't in sensorhistory, and it isn't dead, it's a new node.
    if sensorhistory == None:
#        print "A new KaW %d appeared, isDead: %s" % (xb.address_16, str(isDead))
#        print "Got a packet: %s, makes an xb: %s, address: %d, xb.analog_samples: %s" % (packet.encode("hex"), repr(xb), xb.address_16, xb.analog_samples)
	new_calculations = {}
        node = add_node(hab, xb.address_16)
        sensorhistory = sensorhistories.find(xb.address_16)
	new_calculations["TotalWattHr"] = 0
	new_calculations["newamps"] = 0
	new_calculations["newwatts"] = 0
	new_calculations["NumberSamples"] = 0
	addresses[xb.address_16] = new_calculations

    # we'll only store n-1 samples since the first one is usually messed up
    voltagedata = [-1] * (len(xb.analog_samples) - 1)
    ampdata = [-1] * (len(xb.analog_samples ) -1)
    # grab 1 thru n of the ADC readings, referencing the ADC constants

    # Update the interval or assign it 0 if it doesn't exist, yet
    try:
        intervals[xb.address_16] += time.time() - sensorhistory.lasttime
    except KeyError:
        intervals[xb.address_16] = 0
    sensorhistory.lasttime = time.time() 


    if (options.debug != None):       # for debugging sometimes we only want one
        print xb.address_16


    new_calculations = calculations(voltagedata, ampdata, xb.analog_samples, xb.address_16, sensorhistory)

    if (int(options.interval) < intervals[xb.address_16]):
        
        new_calculations["newamps"] = new_calculations["newamps"]/new_calculations["NumberSamples"]
        new_calculations["newwatts"] = new_calculations["newwatts"]/new_calculations["NumberSamples"]

        node = add_node(hab, xb.address_16)

        resource = bionet_node_get_resource_by_id(node, "Amps")
        if (resource == None):
            logger.error("Error no such resource - Amps")
        else:
            bionet_resource_set_float(resource, new_calculations["newamps"], None)

        resource = bionet_node_get_resource_by_id(node, "Watts")
        if (resource == None):
            logger.error("Error no such resource - Watts")
        else:
            bionet_resource_set_float(resource, new_calculations["newwatts"], None)

        resource = bionet_node_get_resource_by_id(node, "KW-Hours")
        if (resource == None):
            logger.error("Error no such resource - W-Hours")
        else:
            bionet_resource_set_float(resource, new_calculations["WattHr"], None)

        resource = bionet_node_get_resource_by_id(node, "Total-KW-Hours")
        if (resource == None):
            logger.error("Error no such resource - Total-KW-Hours")
        else:
            bionet_resource_set_float(resource, new_calculations["TotalWattHr"], None)
        hab_report_datapoints (node)
        new_calculations["newwatts"] = 0
        new_calculations["newamps"] = 0
    	new_calculations["WattHr"] = 0
        intervals[xb.address_16] = 0
        new_calculations["NumberSamples"] = 0

while True:
    update_graph(None)
	

