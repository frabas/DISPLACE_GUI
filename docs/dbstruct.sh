#!/bin/sh

echo "Database Structure"
echo "=================="
echo " "

for i in `sqlite3 $1 .tables` ; do
    echo "TABLE: $i"
    echo -n "Schema: "
    sqlite3 $1 ".schema $i" ;
    echo " "
done
