#!/bin/bash

ORIG_DIR=$PWD

#make the new database and start the BDM, but only listen to RANDOM HABs.
echo "Killing any running BDM servers..."
killall bionet-data-manager lt-bionet-data-manager > /dev/null 2> /dev/null
sleep 1
rm -f $ORIG_DIR/bdm.db
cd ../../data-manager/server
mv bdm.db bdm.db-old
echo "Making DB file..."
./make-db.sh > /dev/null
mv bdm.db $ORIG_DIR/.
mv bdm.db-old bdm.db
cd $ORIG_DIR
echo "Starting BDM server..."
bionet-data-manager -f ./bdm.db -r "RANDOM.*.*:*" 2> /dev/null &
sleep 1

#run a Random HAB and output the test data here
cd ../../hab/random-py
echo "Running Random-HAB..."
./random-hab.py -i tester1 -t $ORIG_DIR/random.out -l 30 > /dev/null 2> /dev/null
sleep 1

#grab data from the BDM
YEAR=`date +%Y`
cd ../../data-manager/scripts
echo "Running BDM-Client..."
./bdm-client.py -o chrono -s localhost -b "$YEAR-01-01 00:00:00" -e "$YEAR-12-31 12:59:59" -r "RANDOM.*.*:*" > $ORIG_DIR/bdm.out

#stop the BDM
echo "Killing BDM server..."
killall bionet-data-manager

cd $ORIG_DIR

echo `diff random.out bdm.out | wc -l` "differences."