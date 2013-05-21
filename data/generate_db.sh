#!/bin/sh

rm database.sqlite
cat dump.sql | sqlite3 database.sqlite
