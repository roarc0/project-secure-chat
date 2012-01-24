AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

bin_PROGRAMS = client server
client_SOURCES = src/shared/function.h src/shared/function.c   \
                  src/shared/command.h src/shared/command.c \
		  src/shared/file.c src/shared/file.h \
		  src/shared/utility.c src/shared/utility.h \
                  src/shared/time.c src/shared/time.h \
		  src/shared/crypt.c src/shared/crypt.h \
                  src/client/socket-client.c src/client/socket-client.h \
		  src/client/command-client.c src/client/command-client.h \
		  src/client/function-client.c src/client/function-client.h \
                  src/client/main.c

server_SOURCES = src/shared/function.h src/shared/function.c   \
                  src/shared/command.h src/shared/command.c \
		  src/shared/file.c src/shared/file.h \
		  src/shared/utility.c src/shared/utility.h \
                  src/shared/time.c src/shared/time.h \
		  src/shared/crypt.c src/shared/crypt.h \
                  src/server/socket-server.c src/client/socket-server.h \
		  src/server/command-server.c src/server/command-server.h \
		  src/server/function-server.c src/server/function-server.h \
	          src/server/thread.c src/server/tread.h \
	          src/shared/list.c src/shared/list.h \
	          src/server/archive.c src/server/archive.h \
                  src/server/main.c


dist_noinst_SCRIPTS = autogen.sh

