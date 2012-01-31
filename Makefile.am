AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}
bin_PROGRAMS = client server

AM_CPPFLAGS = $(GLIB_CFLAGS) $(GTK_CFLAGS)

client_LDADD = $(GLIB_LIBS) $(GTK_LIBS)
client_SOURCES = src/shared/networking/socket.h src/shared/networking/socket.cpp   \
                 src/shared/utility/logger.h src/shared/utility/logger.cpp         \
                 src/shared/common.h src/shared/common.cpp                         \
                 src/shared/utility/file.h src/shared/utility/file.cpp             \
                 src/shared/config.h src/shared/config.cpp                         \
                 src/shared/threading/thread.h src/shared/threading/thread.cpp     \
                 src/client/client-config.h src/client/client-config.cpp           \
                 src/client/client-core.h src/client/client-core.cpp               \
                 src/client/gui.h src/client/gui.cpp                               \
                 src/client/main.cpp

server_SOURCES = src/shared/networking/socket.h src/shared/networking/socket.cpp   \
                 src/shared/utility/logger.h src/shared/utility/logger.cpp         \
                 src/shared/common.h src/shared/common.cpp                         \
                 src/shared/utility/file.h src/shared/utility/file.cpp             \
                 src/shared/config.h src/shared/config.cpp                         \
                 src/shared/threading/thread.h src/shared/threading/thread.cpp     \
                 src/server/server-config.h src/server/server-config.cpp           \
                 src/server/server-core.h src/server/server-core.cpp               \
                 src/server/session.h src/server/session/session.cpp               \
                 src/server/session-manager.h src/server/session/session-manager.cpp \
                 src/server/main.cpp

dist_noinst_SCRIPTS = autogen.sh

