#!/bin/bash

ORIG_DIR=$PWD

#make the new database and start the BDM, but only listen to RANDOM HABs.
echo "Killing any running BDM servers..."
killall bionet-data-manager lt-bionet-data-manager > /dev/null 2> /dev/null
sleep 1
rm -f $ORIG_DIR/bdm.db
cd ../../../data-manager/server
mv bdm.db bdm.db-old
echo "Making DB file..."
./make-db.sh > /dev/null
mv bdm.db $ORIG_DIR/.
mv bdm.db-old bdm.db
cd $ORIG_DIR
echo "Starting BDM server..."
../../../data-manager/server/bionet-data-manager -f $ORIG_DIR/bdm.db &
../../../client/watcher/bionet-watcher > watcher.out &
sleep 1

#run a Random HAB and output the test data here
cd ../../../hab/random
echo "Running Random-HAB..."
./random-hab -i tester1 --output-mode bdm-client 2> $ORIG_DIR/random.out &
sleep 35
killall lt-random-hab 

#grab data from the BDM
YEAR=`date +%Y`
cd ../../../data-manager/scripts
echo "Running BDM-Client..."
./bdm-client.py -o chrono -s localhost -b "$YEAR-01-01 00:00:00" -e "$YEAR-12-31 12:59:59" -r "*.*.*:*" > $ORIG_DIR/bdm.out

#stop the BDM
echo "Killing BDM server..."
killall lt-bionet-data-manager
killall lt-bionet-watcher

cd $ORIG_DIR

echo `diff random.out bdm.out | wc -l` "differences."