#!/bin/sh

GDB="debug.gdb"
BINARY="$1"
BINARY_ARGS=""

DLOG="debug-`date`.log"

echo -e "run\nbt\nbt full\ninfo thread\nthread apply all backtrace full" > $GDB

gdb -x $GDB --batch --args ./$BINARY $BINARY_ARGS > $DLOG

