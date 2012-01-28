AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}
bin_PROGRAMS = client server

AM_CPPFLAGS = $(GLIB_CFLAGS) $(GTK_CFLAGS)

client_LDADD = $(GLIB_LIBS) $(GTK_LIBS)
client_SOURCES = src/shared/networks/socket.h src/shared/networks/socket.cpp   \
                 src/shared/logger/logger.h src/shared/logger/logger.cpp       \
                 src/shared/common.h src/shared/common.cpp                     \
                 src/shared/utility/file.h src/shared/utility/file.cpp         \
                 src/client/config-client.h src/client/config-client.cpp       \
                 src/client/gui.h src/client/gui.cpp                           \
                 src/client/main.cpp

server_SOURCES = src/shared/networks/socket.h src/shared/networks/socket.cpp   \
                 src/shared/logger/logger.h src/shared/logger/logger.cpp       \
                 src/shared/common.h src/shared/common.cpp                     \
                 src/shared/utility/file.h src/shared/utility/file.cpp         \
                 src/sever/config-server.h src/server/config-server.cpp        \
                 src/server/main.cpp

dist_noinst_SCRIPTS = autogen.sh

