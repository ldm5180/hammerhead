#!/usr/bin/python
#
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.
#

import sqlite3
import optparse
import os.path

from time import gmtime, strftime


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

        parser.add_option("--sync-info", 
	    dest="sync_info", 
            action="store_true",
            help="Display the sync info",
            default=0)


        parser.add_option("--dump-debug", 
	    dest="dump_debug", 
            action="store_true",
            help="Dump debug output",
            default=0)



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
        str = "%s.%06d,%s.%s.%s:%s,%s" % ( \
                strftime("%Y-%m-%d %H:%M:%S", gmtime(int(row[0]))),
                row[1], 
                row[3],
                row[4],
                row[5],
                row[6],
                row[7])
        return str

    def watcher_output_formatter(self, cursor, row):
        str = "%s.%06d,%s/%s.%s.%s:%s,%s" % ( \
                strftime("%Y-%m-%d %H:%M:%S", gmtime(int(row[0]))),
                row[1], 
                row[2], #BDM-id
                row[3],
                row[4],
                row[5],
                row[6],
                row[7])
        return str
        

    def debug_output_formatter(self, cursor, row):
        str = "[%04d] %s.%06d,%s/%s.%s.%s:%s,%s" % ( \
		row[8],
                strftime("%Y-%m-%d %H:%M:%S", gmtime(int(row[0]))),
                row[1], 
                row[2], #BDM-id
                row[3],
                row[4],
                row[5],
                row[6],
                row[7])
        return str

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

        sql = '''
        SELECT d.Timestamp_Sec, d.Timestamp_Usec, b.BDM_ID, 
                  h.HAB_Type, h.HAB_ID, n.Node_ID, r.Resource_ID, d.value, d.Entry_NUm
        FROM BDMs b, Datapoints d, Resources r , Nodes n, Hardware_Abstractors h, 
             Resource_Data_types t, Resource_Flavors f
        WHERE d.Resource_key = r.Key
          AND d.BDM_Key = b.Key
          AND r.Node_Key = n.Key
          AND r.Flavor_Key = f.Key
          AND r.Data_Type_Key = t.Key
        ORDER BY d.Timestamp_Sec asc, d.Timestamp_usec asc 
          '''

        self.print_query(sql, formatter)


    def list_missing_metadata(self):

        sql = '''
        SELECT d.Resource_key
        FROM Datapoints d
        WHERE d.Resource_key NOT IN
            (SELECT distinct key from Resources)
          '''

        print "Missing metadata for resources:"

        def fmt(cur,row): return "  Resource with key %08x" % (row[0])
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


    def sync_info(self):

        sql = '''
        SELECT BDM_ID,Last_Sync_Metadata,Last_Sync_Datapoints from BDMs
          '''

        def fmt(cur,row): 
            return "%s %d %d" % (row[0], row[1] or -1, row[2] or -1)

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

    if inspector.options.sync_info:
        inspector.sync_info()

    if inspector.options.dump_debug:
        inspector.simulate_client_output(inspector.debug_output_formatter)



    #inspector.sql_shell()


if __name__ == "__main__":
    main()



