#!/bin/bash
#
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.
#


set -x
set -e

DB=bdm-test-$$.db
SQL="sqlite3 $DB"

# make a fresh, empty database based on the current schema
if [ -f "$DB" ]; then
    echo "DB $DB already exists, quitting"
    exit 1
fi

$SQL < schema

$SQL .schema




# show the Resource Data Types and Flavors tables
echo "Resource Data Types:"
$SQL "SELECT * FROM Resource_Data_Types;"

echo "Resource Flavors:"
$SQL "SELECT * FROM Resource_Flavors;"




# 
# test the HAB table
#

echo insert some HABs
$SQL "INSERT INTO Hardware_Abstractors VALUES ( NULL, 'test-type-0', 'test-id-0' )"
$SQL "INSERT INTO Hardware_Abstractors VALUES ( NULL, 'test-type-0', 'test-id-1' )"
$SQL "INSERT INTO Hardware_Abstractors VALUES ( NULL, 'test-type-1', 'test-id-0' )"
$SQL "INSERT INTO Hardware_Abstractors VALUES ( NULL, 'test-type-1', 'test-id-1' )"

# show what we have so far
$SQL "SELECT * from Hardware_Abstractors;"


echo HABs must be unique
if $SQL "INSERT INTO Hardware_Abstractors VALUES ( NULL, 'test-type-1', 'test-id-1' )"; then
    exit 1
fi


echo "it's ok if a HAB is already in the table"
$SQL "INSERT OR IGNORE INTO Hardware_Abstractors VALUES ( NULL, 'test-type-1', 'test-id-1' )"




#
# test the Node table
#

echo insert some Nodes

HAB_KEY=$($SQL "SELECT Key FROM Hardware_Abstractors WHERE ( HAB_Type='test-type-0' AND HAB_ID='test-id-0' )")
$SQL "INSERT INTO Nodes VALUES ( NULL, '$HAB_KEY', 'test-node-0' );"
$SQL "INSERT INTO Nodes VALUES ( NULL, '$HAB_KEY', 'test-node-1' );"

HAB_KEY=$($SQL "SELECT Key FROM Hardware_Abstractors WHERE ( HAB_Type='test-type-1' AND HAB_ID='test-id-1' )")
$SQL "INSERT INTO Nodes VALUES ( NULL, '$HAB_KEY', 'test-node-0' );"
$SQL "INSERT INTO Nodes VALUES ( NULL, '$HAB_KEY', 'test-node-1' );"

# show what we have so far
$SQL "SELECT * from Nodes;"

$SQL "SELECT Hardware_Abstractors.HAB_Type, Hardware_Abstractors.HAB_ID, Nodes.Node_ID from Hardware_Abstractors, Nodes WHERE Nodes.HAB_Key=Hardware_Abstractors.Key;"


echo "within a HAB, Nodes must be unique"
HAB_KEY=$($SQL "SELECT Key FROM Hardware_Abstractors WHERE ( HAB_Type='test-type-1' AND HAB_ID='test-id-1' )")
if $SQL "INSERT INTO Nodes VALUES ( NULL, '$HAB_KEY', 'test-node-0' );"; then
    exit 1
fi


echo "it's ok if a Node is already in the table"
$SQL "
    INSERT
    OR IGNORE
    INTO Nodes
    SELECT
        NULL, Hardware_Abstractors.Key, 'test-node-0'
        FROM Hardware_Abstractors
        WHERE
            HAB_Type='test-type-1'
            AND HAB_ID='test-id-1'
    ;
    "




#
# test the Resource table
#

echo insert some Resources

HAB_KEY=$($SQL "SELECT Key FROM Hardware_Abstractors WHERE ( HAB_Type='test-type-0' AND HAB_ID='test-id-0' )")
NODE_KEY=$($SQL "SELECT Key FROM Nodes WHERE ( HAB_Key='$HAB_KEY' AND Node_ID='test-node-0' )")
DATA_TYPE_KEY=$($SQL "SELECT Key FROM Resource_Data_Types WHERE ( Data_Type = 'binary' )")
FLAVOR_KEY=$($SQL "SELECT Key FROM Resource_Flavors WHERE ( Flavor = 'actuator' )")

$SQL "INSERT INTO Resources VALUES ( X'12345678ABCDEF00', '$NODE_KEY', 'test-resource-0', '$DATA_TYPE_KEY', '$FLAVOR_KEY' );"
$SQL "INSERT INTO Resources VALUES ( X'C0FFEE0000000000', '$NODE_KEY', 'test-resource-1', '$DATA_TYPE_KEY', '$FLAVOR_KEY' );"

# show what we have so far
echo "Resources table:"
$SQL "SELECT quote(Key), Node_Key, Resource_ID, Data_Type_Key, Flavor_Key from Resources;"

echo "Resources (decoded):"

$SQL "
    SELECT
        Hardware_Abstractors.HAB_Type,
        Hardware_Abstractors.HAB_ID,
        Nodes.Node_ID,
        Resources.Resource_ID,
        Resource_Data_Types.Data_Type,
        Resource_Flavors.Flavor
    FROM
        Hardware_Abstractors,
        Nodes,
        Resources,
        Resource_Data_Types,
        Resource_Flavors
    WHERE
        (
            Nodes.HAB_Key = Hardware_Abstractors.Key
            AND Resources.Node_Key = Nodes.Key
            AND Resource_Data_Types.Key = Resources.Data_Type_Key
            AND Resource_Flavors.Key = Resources.Flavor_Key
        )
    ;
"


echo "within a Node, Resources must be unique"

HAB_KEY=$($SQL "SELECT Key FROM Hardware_Abstractors WHERE ( HAB_Type='test-type-0' AND HAB_ID='test-id-0' )")
NODE_KEY=$($SQL "SELECT Key FROM Nodes WHERE ( HAB_Key='$HAB_KEY' AND Node_ID='test-node-0' )")
DATA_TYPE_KEY=$($SQL "SELECT Key FROM Resource_Data_Types WHERE ( Data_Type = 'binary' )")
FLAVOR_KEY=$($SQL "SELECT Key FROM Resource_Flavors WHERE ( Flavor = 'actuator' )")

# same metadata & same key
if $SQL "INSERT INTO Resources VALUES ( X'C0FFEE0000000000', '$NODE_KEY', 'test-resource-1', '$DATA_TYPE_KEY', '$FLAVOR_KEY' );"; then
    exit 1
fi


echo "duplicated Resource Name is ok, as long as some of the other Resource metadata changes"
DATA_TYPE_KEY=$($SQL "SELECT Key FROM Resource_Data_Types WHERE ( Data_Type = 'string' )")
FLAVOR_KEY=$($SQL "SELECT Key FROM Resource_Flavors WHERE ( Flavor = 'parameter' )")
$SQL "INSERT INTO Resources VALUES ( X'0000000000000001', '$NODE_KEY', 'test-resource-1',  '$DATA_TYPE_KEY', '$FLAVOR_KEY' );"


echo "different Resource metadata wih fingerprint collision is a failure"
DATA_TYPE_KEY=$($SQL "SELECT Key FROM Resource_Data_Types WHERE ( Data_Type = 'float' )")
FLAVOR_KEY=$($SQL "SELECT Key FROM Resource_Flavors WHERE ( Flavor = 'sensor' )")
if $SQL "INSERT INTO Resources VALUES ( X'C0FFEE0000000000', '$NODE_KEY', 'other-resource-id',  $DATA_TYPE_KEY, $FLAVOR_KEY );"; then
    exit 1
fi


echo "it's ok if a Resource is already in the table"
$SQL "
    INSERT
        OR IGNORE
        INTO Resources
        SELECT
            X'C0FFEE0000000000', Nodes.Key, 'test-resource-1', Resource_Data_Types.Key, Resource_Flavors.Key
            FROM Hardware_Abstractors, Nodes, Resource_Data_Types, Resource_Flavors
            WHERE
                Hardware_Abstractors.HAB_Type = 'test-type-0'
                AND Hardware_Abstractors.HAB_ID = 'test-id-0'
                AND Nodes.HAB_Key = Hardware_Abstractors.Key
                AND Nodes.Node_ID = 'test-node-0'
                AND Resource_Data_Types.Data_Type = 'binary'
                AND Resource_Flavors.Flavor = 'actuator'
        ;
        "




#
# some transactions
#

echo "testing some transactions"

$SQL "
    BEGIN TRANSACTION;

    INSERT
        OR IGNORE
        INTO Hardware_Abstractors
        VALUES ( NULL, 'trans-hab-type', 'trans-hab-id' )
    ;

    INSERT
        OR IGNORE
        INTO Nodes
        SELECT
            NULL, Hardware_Abstractors.Key, 'trans-node-id'
            FROM Hardware_Abstractors
            WHERE
                HAB_Type='trans-hab-type'
                AND HAB_ID='trans-hab-id'
    ;
        
    INSERT
        OR IGNORE
        INTO Resources
        SELECT
            NULL, Nodes.Key, 'trans-resource-id', Resource_Data_Types.Key, Resource_Flavors.Key
            FROM Hardware_Abstractors, Nodes, Resource_Data_Types, Resource_Flavors
            WHERE 
                Hardware_Abstractors.HAB_Type='trans-hab-type'
                AND Hardware_Abstractors.HAB_ID='trans-hab-id'
                AND Nodes.HAB_Key=Hardware_Abstractors.Key
                AND Nodes.Node_ID='trans-node-id'
                AND Resource_Data_Types.Data_Type='int16'
                AND Resource_Flavors.Flavor='actuator'
    ;

    COMMIT;
    "



./dump-db $DB


echo "tests PASS!"

# clean up the test db
rm $DB

