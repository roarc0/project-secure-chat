#!/bin/sh

rm database.sqlite.dist
cat dump.sql | sqlite3 database.sqlite.dist
