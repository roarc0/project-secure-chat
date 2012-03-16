# Copyright (C) 2008-2010 Trinity <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

file(GLOB_RECURSE sources_Cryptography cryptography/*.cpp cryptography/*.h)
file(GLOB_RECURSE sources_Execution execution/*.cpp execution/*.h)
file(GLOB_RECURSE sources_Networking networking/*.cpp networking/*.h)
file(GLOB_RECURSE sources_Queues queues/*.cpp queues/*.h)
file(GLOB_RECURSE sources_Session session/*.cpp session/*.h)
file(GLOB_RECURSE sources_Threading threading/*.cpp threading/*.h)
file(GLOB_RECURSE sources_Utility utility/*.cpp utility/*.h)

file(GLOB sources_localdir *.cpp *.h)

#
# Build shared sourcelist
#


set(shared_STAT_SRCS
  ${sources_Cryptography}
  ${sources_Execution}
  ${sources_Networking}
  ${sources_Queues}
  ${sources_Session}
  ${sources_Threading}
  ${sources_Utility}
  ${sources_localdir}
)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/cryptography
  ${CMAKE_CURRENT_SOURCE_DIR}/execution
  ${CMAKE_CURRENT_SOURCE_DIR}/networking
  ${CMAKE_CURRENT_SOURCE_DIR}/packets
  ${CMAKE_CURRENT_SOURCE_DIR}/queues
  ${CMAKE_CURRENT_SOURCE_DIR}/session
  ${CMAKE_CURRENT_SOURCE_DIR}/threading
  ${CMAKE_CURRENT_SOURCE_DIR}/utility
  ${OPENSSL_INCLUDE_DIR}
)

add_library(shared STATIC ${shared_STAT_SRCS})
