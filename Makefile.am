AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}
bin_PROGRAMS = server

AM_CPPFLAGS = $(GLIB_CFLAGS) $(GTK_CFLAGS)

client_LDADD = $(GLIB_LIBS) $(GTK_LIBS)
#client_SOURCES = src/shared/common.h src/shared/common.cpp                                                      \
#                 src/shared/utility/timer.h src/shared/utility/timer.cpp                                        \
#                 src/shared/utility/file.h src/shared/utility/file.cpp                                          \
#                 src/shared/utility/config.h src/shared/utility/config.cpp                                      \
 #                src/shared/utility/logger.h src/shared/utility/logger.cpp                                      \
  #               src/shared/networking/socket-exception.h src/shared/networking/socket-exception.cpp            \
   #              src/shared/networking/socket-base.h src/shared/networking/socket-base.cpp                      \
    #             src/shared/networking/packet.h src/shared/networking/packet.cpp                                \
     #            src/shared/threading/thread.h src/shared/threading/thread.cpp                                  \
      #           src/shared/execution/command-manager.h src/shared/execution/command-manager.cpp                \
       #          src/shared/execution/command.h src/shared/execution/command.cpp                                \
       #          src/shared/execution/functions.h src/shared/execution/functions.cpp                            \
        #         src/shared/cryptography/crypto.h src/shared/cryptography/crypto.cpp                            \
         #        src/client/networking/socket-client.h src/client/networking/socket-client.cpp                  \
          #       src/client/client-config.h src/client/client-config.cpp                                        \
           #      src/client/client-core.h src/client/client-core.cpp                                            \
            #     src/client/gui.h src/client/gui.cpp                                                            \
             #    src/client/main.cpp
#src/shared/execution/*.h src/shared/execution/*.cpp 
#src/server/execution/*.h src/server/execution/*.cpp 
#src/shared/networking/*.h src/shared/networking/*.cpp  
#src/server/networking/*.h src/server/networking/*.cpp         


server_SOURCES = src/shared/common.cpp                                                      \
                 src/shared/utility/*.cpp                                                \
                 src/shared/threading/*.cpp                                            \
                 src/shared/cryptography/crypto.cpp                            \
                 src/server/session/*.cpp                                                \
                 src/server/channel/*.cpp                                                \
                 src/server/database/database-manager.cpp                \
                 src/server/server-config.cpp                                        \
                 src/server/server-core.cpp                                            \
                 src/server/main.cpp

dist_noinst_SCRIPTS = autogen.sh

