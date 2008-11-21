#!/bin/bash
set -e

DB=bdm.db
SQL="sqlite3 $DB"

# make a fresh, empty database based on the current schema
if [ -f "$DB" ]; then
    echo "DB $DB already exists, quitting"
    exit 1
fi

$SQL < schema


# show the schema we ended up with
$SQL .schema

# show the Resource Data Types and Flavors tables
echo "Resource Data Types:"
$SQL "SELECT * FROM Resource_Data_Types;"

echo "Resource Flavors:"
$SQL "SELECT * FROM Resource_Flavors;"


