#!/bin/sh

echo "* clean"

if [ -e server ] ; then
    cp server server_tmp
fi

if [ -e client ] ; then
    cp client client_tmp
fi

if [ -e "Makefile" ] ; then
    make clean > /dev/null 2>&1
fi

if [ -e server_tmp ] ; then
    mv server_tmp server
fi

if [ -e client_tmp ] ; then
    mv client_tmp client
fi

rm -f *~

find -name *~ -exec rm -f {} \; > /dev/null 2>&1
find -name *.o -exec rm -f {} \; > /dev/null 2>&1

find -name .dirstamp -exec rm -f {} \; > /dev/null 2>&1
find -name .deps -exec rm -rf {} \; > /dev/null 2>&1

rm -rf autom4te.cache/ stamp-h1 config.status install-sh \
        config.log aclocal.m4 Makefile Makefile.in configure config.status config.h \
       config.h.in depcomp missing  INSTALL *.log *.gdb compile \ > /dev/null 2>&1

