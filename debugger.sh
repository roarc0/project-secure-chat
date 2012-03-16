#!/bin/bash

######################
GDB="psc.gdb"

TRACE_BEGIN_STRING="SIGSEGV"
TRACE_FILE="crashlog.log"
SYSLOG="system.log"
SYSERR="error.log"
######################

DATE=$(date)
echo "Starting psc server ($DATE)"
echo -e "run\nbt\nbt full\ninfo thread\nthread apply all backtrace full" > $GDB

rm $SYSLOG $SYSERR > /dev/null 2>&1
gdb -x $GDB --batch --args ./$1 >$SYSLOG 2>$SYSERR
grep -B 10 -A 1800 "$TRACE_BEGIN_STRING" "$SYSLOG" > "$TRACE_FILE"

