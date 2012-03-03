AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}
bin_PROGRAMS = client server

AM_CPPFLAGS = $(GLIB_CFLAGS) $(GTK_CFLAGS)

client_LDADD = $(GLIB_LIBS) $(GTK_LIBS)
client_SOURCES = src/shared/networking/socket-exception.h src/shared/networking/socket-exception.cpp            \
                 src/shared/networking/packet.h src/shared/networking/packet.cpp                                \
                 src/shared/utility/logger.h src/shared/utility/logger.cpp                                      \
                 src/shared/common.h src/shared/common.cpp                                                      \
                 src/shared/utility/time.h src/shared/utility/time.cpp                                          \
                 src/shared/utility/file.h src/shared/utility/file.cpp                                          \
                 src/shared/config.h src/shared/config.cpp                                                      \
                 src/shared/threading/thread.h src/shared/threading/thread.cpp                                  \
                 src/shared/execution/command-manager.h src/shared/execution/command-manager.cpp                \
                 src/shared/execution/command.h src/shared/execution/command.cpp                                \
                 src/shared/execution/functions.h src/shared/execution/functions.cpp                            \
                 src/shared/cryptography/crypto.h src/shared/cryptography/crypto.cpp                            \
                 src/client/networking/socket-client.h src/client/networking/socket-client.cpp                  \
                 src/client/client-config.h src/client/client-config.cpp                                        \
                 src/client/client-core.h src/client/client-core.cpp                                            \
                 src/client/gui.h src/client/gui.cpp                                                            \
                 src/client/main.cpp

server_SOURCES = src/shared/networking/socket-exception.h src/shared/networking/socket-exception.cpp            \
                 src/shared/networking/packet.h src/shared/networking/packet.cpp                                \
                 src/shared/utility/logger.h src/shared/utility/logger.cpp                                      \
                 src/shared/common.h src/shared/common.cpp                                                      \
                 src/shared/utility/time.h src/shared/utility/time.cpp                                          \
                 src/shared/utility/file.h src/shared/utility/file.cpp                                          \
                 src/shared/config.h src/shared/config.cpp                                                      \
                 src/shared/threading/thread.h src/shared/threading/thread.cpp                                  \
                 src/shared/execution/command-manager.h src/shared/execution/command-manager.cpp                \
                 src/shared/execution/command.h src/shared/execution/command.cpp                                \
                 src/shared/execution/functions.h src/shared/execution/functions.cpp                            \
                 src/shared/cryptography/crypto.h src/shared/cryptography/crypto.cpp                            \
                 src/server/networking/socket-server.h src/server/networking/socket-server.cpp                  \
                 src/server/execution/command-manager-server.h src/server/execution/command-manager-server.cpp  \
                 src/server/execution/functions-server.h src/server/execution/functions-server.cpp              \
                 src/shared/session/user-session.h src/shared/session/user-session.cpp                          \
                 src/server/session/execution-threads.h src/server/session/execution-threads.cpp                \
                 src/server/session/network-threads.h src/server/session/network-threads.cpp                    \
                 src/server/session/session-manager.h src/server/session/session-manager.cpp                    \
                 src/server/channel/channel-manager.h src/server/channel/channel-manager.cpp                    \
                 src/server/database/database-manager.h src/server/database/database-manager.cpp                \
                 src/server/server-config.h src/server/server-config.cpp                                        \
                 src/server/server-core.h src/server/server-core.cpp                                            \
                 src/server/main.cpp

dist_noinst_SCRIPTS = autogen.sh

