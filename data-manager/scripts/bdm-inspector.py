#!/usr/bin/python
#
# vim: sw=4 sta expandtab
#
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


import sqlite3
import optparse
import os.path

from time import gmtime, strftime

def val_to_str(val,type_str):
    if(type_str == 'float'):
        str = "%.7g" % val
    if(type_str == 'double'):
        str = "%.14g" % val
    elif(type_str.startswith('uint')):
        str = "%u" % val
    elif(type_str.startswith('int')):
        str = "%d" % val
    else:
        str = val

    return str

class BdmInspector:
    dbpath = "bdm.db"
    con = None
    cur = None

    def __init__(self):
        self.config_argv()

        self.con = sqlite3.connect(self.dbpath)
        self.con.isolation_level = None
        self.cur = self.con.cursor()

    def __destroy__(self):
        print "Running destroy"
        self.con.close()



    def config_argv(self):
        """ Init this opbject with argv """
        usage = """usage: %prog [options]

This script directly accesses the database file. All operations are
safe to use on a database file that is in-use by a bionet-data-manager 
instance
"""

        parser = optparse.OptionParser(usage=usage)

        parser.add_option("-f", "--file", 
            dest="dbfile", 
            type="string",
            help="Database file path",
            default="bdm.db")

        parser.add_option("--client", 
	    dest="sim_client", 
            action="store_true",
            help="dump the datapose in bdm-client form",
            default=0)

        parser.add_option("--watcher", 
	    dest="sim_watcher", 
            action="store_true",
            help="dump the datapose in bdm-watcher form",
            default=0)

        parser.add_option("--missing", 
	    dest="list_missing", 
            action="store_true",
            help="List the resource keys that are missing",
            default=0)

        parser.add_option("--resources", 
	    dest="list_resources", 
            action="store_true",
            help="List the resource keys that are present",
            default=0)


        parser.add_option("--dump-debug", 
	    dest="dump_debug", 
            action="store_true",
            help="Dump debug output",
            default=0)

        parser.add_option("--start-seq", 
	    dest="start_seq", 
            type="int",
            help="Only return events after this sequence number")

        parser.add_option("--end-seq", 
	    dest="end_seq", 
            type="int",
            help="Only return events after this sequence number")

        parser.add_option("--filter", 
	    dest="filter", 
            help="Apply this resource name filter",
            default=None)

        parser.add_option("--lookup-resource", 
	    dest="lookup_resource", 
            type="string",
            help="Lookup resource with key in hex",
            default=None)

        parser.add_option("--lookup-node", 
	    dest="lookup_node", 
            type="string",
            help="Lookup node with key in hex",
            default=None)


        (options, args) = parser.parse_args()

        if len(args) > 0:
            print "Extra args specified: " + " ".join(args)
            raise Exception('Extra Args')

        self.dbpath = os.path.abspath(options.dbfile)
        if not os.path.exists(self.dbpath):
            print "DB File '%s' could not be found" % self.dbpath
            raise Exception("File not Found")

    	self.options = options


    def sql_shell(self):

        buffer = ""

        print "Enter your SQL commands to execute in sqlite3."
        print "Enter a blank line to exit."

        while True:
            line = raw_input()
            if line == "":
                break
            buffer += line
            if sqlite3.complete_statement(buffer):
                try:
                    buffer = buffer.strip()
                    self.cur.execute(buffer)

                    if buffer.lstrip().upper().startswith("SELECT"):
                        print "Results: "
                        print self.cur.fetchall()
                except sqlite3.Error, e:
                    print "An error occurred:", e.args[0]
                buffer = ""



    def client_output_formatter(self, cursor, row):
	time_str = strftime("%Y-%m-%d %H:%M:%S", gmtime(int(row[10])))
	if(row[14]):
		islost = "-"
	else:
		islost = "+"
	if(row[8]):
		# Has datapoint time
		dp_time_str = strftime("%Y-%m-%d %H:%M:%S", gmtime(int(row[8])))
		id_str = "%s.%s.%s:%s,%s %s %s @ %s" % \
                        (row[0], row[1], row[2], row[6], row[4], row[5], val_to_str(row[7], row[4]), dp_time_str)

		str = "%s.%06d,+D,%s" % ( \
			time_str, row[11],
			id_str)
	elif(row[2]):
		# Has node-id
		if(row[6]):
			id_str = "%s.%s.%s:%s,%s %s" % (row[0], row[1], row[2], row[6], row[4], row[5])
		else:
			id_str = "%s.%s.%s" % (row[0], row[1], row[2])

		str = "%s.%06d,%sN,%s" % ( \
			time_str, row[11],
			islost, 
			id_str)
	else:
		# Must be  new/lost HAB
		id_str = "%s.%s" % (row[0], row[1])

		str = "%s.%06d,%sH,%s" % ( \
			time_str, row[11],
			islost, 
			id_str)
        return str

    def watcher_output_formatter(self, cursor, row):
	time_str = strftime("%Y-%m-%d %H:%M:%S", gmtime(int(row[10])))
	if(row[14]):
		islost = "-"
	else:
		islost = "+"
	if(row[8]):
		# Has datapoint time
		dp_time_str = strftime("%Y-%m-%d %H:%M:%S", gmtime(int(row[8])))
		id_str = "%s.%s.%s:%s,%s %s %s @ %s" % \
                        (row[0], row[1], row[2], row[6], row[4], row[5], val_to_str(row[7], row[4]), dp_time_str)

		str = "%s.%06d,+D,%s,%s" % ( \
			time_str, row[11],
			id_str, 
                        row[13])
	elif(row[2]):
		# Has node-id
		if(row[6]):
			id_str = "%s.%s.%s:%s,%s %s" % (row[0], row[1], row[2], row[6], row[4], row[5])
		else:
			id_str = "%s.%s.%s," % (row[0], row[1], row[2])

		str = "%s.%06d,%sN,%s,%s" % ( \
			time_str, row[11],
			islost, 
			id_str,
                        row[13])
	else:
		# Must be  new/lost HAB
		id_str = "%s.%s" % (row[0], row[1])

		str = "%s.%06d,%sH,%s,,%s" % ( \
			time_str, row[11],
			islost, 
			id_str,
                        row[13])
        return str

    def debug_output_formatter(self, cursor, row):
	time_str = strftime("%Y-%m-%d %H:%M:%S", gmtime(int(row[10])))
	if(row[14]):
		islost = "-"
	else:
		islost = "+"
	if(row[8]):
		# Has datapoint time
		dp_time_str = strftime("%Y-%m-%d %H:%M:%S", gmtime(int(row[8])))
		id_str = "%s.%s.%s:%s,%s %s %s @ %s" % \
                        (row[0], row[1], row[2], row[6], row[4], row[5], val_to_str(row[7], row[4]), dp_time_str)

		str = "%s.%06d,+D,%s,%s" % ( \
			time_str, row[11],
			id_str, 
                        row[13])
	elif(row[2]):
		# Has node-id
		if(row[6]):
			id_str = "%s.%s.%s:%s,%s %s" % (row[0], row[1], row[2], row[6], row[4], row[5])
		else:
			id_str = "%s.%s.%s," % (row[0], row[1], row[2])

		str = "%s.%06d,%sN,%s,%s" % ( \
			time_str, row[11],
			islost, 
			id_str,
                        row[13])
	else:
		# Must be  new/lost HAB
		id_str = "%s.%s" % (row[0], row[1])

		str = "%s.%06d,%sH,%s,,%s" % ( \
			time_str, row[11],
			islost, 
			id_str,
                        row[13])
        return "[%04d] %s" % (row[12], str)
        

    def print_query(self,sql,formatter):

        try:
            buffer = sql.strip()
            self.cur.execute(buffer)

            while True:
                row = self.cur.fetchone()
                if row == None:
                    break
                print formatter(self.cur,row)

        except sqlite3.Error, e:
            print "An error occurred:", e.args[0]

    def simulate_client_output(self, formatter):

        habs_sql = '''
	  SELECT h.hab_type, h.hab_id, 
                 NULL, NULL,
                 NULL, NULL, NULL,
                 NULL, NULL, NULL,
		 e.timestamp_sec, e.timestamp_usec, e.seq, b.BDM_ID, e.isLost 
	  FROM   Events e, BDMs b, Hardware_Abstractors h
          WHERE  e.recording_bdm=b.key
		 AND e.hab = h.key
          '''

        nodes_sql = '''
	  SELECT h.hab_type, h.hab_id, 
                 n.Node_ID, n.GUID,
                 t.Data_Type, f.Flavor, r.Resource_ID,
                 NULL, NULL, NULL,
		 e.timestamp_sec, e.timestamp_usec, e.seq, b.BDM_ID, e.isLost 
	  FROM   Events e, BDMs b, Hardware_Abstractors h,
                 Nodes n
                  LEFT OUTER JOIN Resources r
                   ON r.Node_Key=n.Key
                  LEFT OUTER JOIN Resource_Data_Types t
                   ON t.Key=r.Data_Type_Key
                  LEFT OUTER JOIN Resource_Flavors f
                   ON f.Key=r.Flavor_Key
          WHERE  e.recording_bdm=b.key
                 AND n.HAB_Key=h.Key
		 AND e.node = n.key
          '''

        dp_sql = '''
	  SELECT h.hab_type, h.hab_id, 
                 n.Node_ID, n.GUID,
                 t.Data_Type, f.Flavor, r.Resource_ID,
                 d.Value, d.Timestamp_Sec, d.Timestamp_USec,
		 e.timestamp_sec, e.timestamp_usec, e.seq, b.BDM_ID, e.isLost 
	  FROM   Events e, BDMs b, Hardware_Abstractors h, 
                 Datapoints d
                  JOIN Resources r
                   ON r.key=d.Resource_Key
                  JOIN Resource_Data_Types t
                   ON t.Key=r.Data_Type_Key
                  JOIN Resource_Flavors f
                   ON f.Key=r.Flavor_Key
                  JOIN Nodes n
                   ON n.key=r.node_key
          WHERE  e.recording_bdm=b.key
                 AND n.HAB_Key=h.Key
		 AND e.datapoint = d.key
          '''

        dp_sql_old = '''
	  SELECT h.hab_type, h.hab_id, 
                 n.Node_ID, n.GUID,
                 t.Data_Type, f.Flavor, r.Resource_ID,
                 d.Value, d.Timestamp_Sec, d.Timestamp_USec,
		 e.timestamp_sec, e.timestamp_usec, e.seq, b.BDM_ID, e.isLost 
	  FROM   Events e, BDMs b, 
                  Hardware_Abstractors h, 
                  Nodes n,
                  Resources r,
                  Resource_Data_Types t,
                  Resource_Flavors f,
                  Datapoints d
          WHERE  e.recording_bdm=b.key
                 AND n.HAB_Key=h.Key
                 AND r.Node_Key=n.key
                 AND t.Key=r.Data_Type_Key
                 AND f.Key=r.Flavor_Key
                 AND d.Resource_Key=r.Key
		 AND e.datapoint = d.key
                 AND (e.seq <= 196 and e.seq >= 133)
          '''

        filter_all = ""

        #Decompose filter, and add WHERE clauses
        if( self.options.filter ):
            parts = self.options.filter.split('/')
            if len(parts) > 1:
                bdm = parts[0]
                pattern = parts[1]
                filter_all = filter_all + ("AND b.BDM_ID = '%s' " % bdm)
            else:
                pattern = parts[0]

            pats = pattern.split('.')
            hab_filter = ""
            node_filter = ""
            dp_filter = ""
            if len(pats) >= 1 and pats[0] != '*':
                hab_filter = "AND h.hab_type = '%s' " % pats[0]
            if len(pats) >= 2 and pats[1] != '*':
                hab_filter = hab_filter + "AND h.hab_id = '%s' " % pats[1]

            if len(pats) >= 3 and pats[2] != '*':
                node_filter = "AND n.node_id = '%s'" % pats[2]

            if len(pats) >= 4 and pats[3] != '*':
                db_filter = "AND r.Resource_ID = '%s' " % pats[3]

            habs_sql = habs_sql + hab_filter
            nodes_sql = nodes_sql + hab_filter + node_filter
            dp_sql = dp_sql + hab_filter + node_filter + dp_filter

        # Add sequence filter to all
        if( self.options.start_seq and self.options.end_seq ):
            filter_all = filter_all + "AND (e.seq <= %d and e.seq >= %d) " % (self.options.end_seq, self.options.start_seq)
        elif( self.options.start_seq ):
            filter_all = filter_all + "AND e.seq >= %d " % (self.options.start_seq)
        elif( self.options.end_seq ):
            filter_all = filter_all + "AND e.seq <= %d " % (self.options.end_seq)



        #sql = "%s ORDER BY e.seq" % (dp_sql)
        sql = "%s %s \nUNION %s %s \nUNION %s %s \nORDER BY e.seq" % (habs_sql, filter_all, nodes_sql, filter_all, dp_sql, filter_all)

        self.print_query(sql, formatter)


    def list_missing_metadata(self):

        sql = '''
        SELECT hex(d.Resource_key)
        FROM Datapoints d
        WHERE d.Resource_key NOT IN
            (SELECT distinct key from Resources)
          '''

        print "Missing metadata for resources:"

        def fmt(cur,row): return "  Resource with key 0x%s " % row[0]
        self.print_query(sql,fmt)


    def list_resources(self):

        sql = '''
        SELECT h.HAB_Type, h.HAB_ID, n.Node_ID, r.Resource_ID, t.Data_Type, f.Flavor
        FROM Resources r, Nodes n, Hardware_Abstractors h, 
             Resource_Data_types t, Resource_Flavors f
        WHERE r.Node_Key = n.Key
          AND r.Flavor_Key = f.Key
          AND r.Data_Type_Key = t.Key
        ORDER BY  h.HAB_Type, h.HAB_ID, n.Node_ID, r.Resource_ID
          '''

        def fmt(cur,row): return "%s.%s.%s:%s (%s,%s)" % row
        self.print_query(sql, fmt)

    def lookup_resource(self, key):
        sql = '''
        SELECT h.HAB_Type, h.HAB_ID, n.Node_ID, r.Resource_ID, t.Data_Type, f.Flavor
        FROM Resources r, Nodes n, Hardware_Abstractors h, 
             Resource_Data_types t, Resource_Flavors f
        WHERE r.Node_Key = n.Key
          AND r.Flavor_Key = f.Key
          AND r.Data_Type_Key = t.Key
          AND r.key = x'%s'
        ORDER BY  h.HAB_Type, h.HAB_ID, n.Node_ID, r.Resource_ID
          ''' % (key)

        def fmt(cur,row): return "%s.%s.%s:%s (%s,%s)" % row
        self.print_query(sql, fmt)

    def lookup_node(self, key):
        sql = '''
        SELECT h.HAB_Type, h.HAB_ID, n.Node_ID
        FROM Nodes n, Hardware_Abstractors h
        WHERE n.Hab_Key = h.Key
        AND   n.GUID = x'%s'
        ORDER BY  h.HAB_Type, h.HAB_ID, n.Node_ID
          ''' % (key)

        def fmt(cur,row): return "%s.%s.%s" % row
        self.print_query(sql, fmt)

def main():

    inspector = BdmInspector()

    if inspector.options.sim_client:
        inspector.simulate_client_output(inspector.client_output_formatter)

    if inspector.options.sim_watcher:
        inspector.simulate_client_output(inspector.watcher_output_formatter)

    if inspector.options.list_missing:
        inspector.list_missing_metadata()

    if inspector.options.list_resources:
        inspector.list_resources()

    if inspector.options.dump_debug:
        inspector.simulate_client_output(inspector.debug_output_formatter)

    if inspector.options.lookup_node:
        inspector.lookup_node(inspector.options.lookup_node)

    if inspector.options.lookup_resource:
        inspector.lookup_resource(inspector.options.lookup_resource)


    #inspector.sql_shell()


if __name__ == "__main__":
    main()


