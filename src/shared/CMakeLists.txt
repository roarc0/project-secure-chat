file(GLOB_RECURSE sources_Crypto crypto/*.cpp crypto/*.h)
file(GLOB sources_Networking networking/*.cpp networking/*.h)
file(GLOB_RECURSE sources_Session session/*.cpp session/*.h)
file(GLOB_RECURSE sources_Threading threading/*.cpp threading/*.h)
file(GLOB_RECURSE sources_Utility utility/*.cpp utility/*.h)
file(GLOB_RECURSE sources_Queues utility/queues/*.cpp utility/queues/*.h)
file(GLOB sources_localdir *.cpp *.h)

set(shared_STAT_SRCS
  ${sources_Crypto}
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
  ${CMAKE_CURRENT_SOURCE_DIR}/crypto
  ${CMAKE_CURRENT_SOURCE_DIR}/networking
  ${CMAKE_CURRENT_SOURCE_DIR}/packets
  ${CMAKE_CURRENT_SOURCE_DIR}/session
  ${CMAKE_CURRENT_SOURCE_DIR}/threading
  ${CMAKE_CURRENT_SOURCE_DIR}/utility
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/queues
  ${OPENSSL_INCLUDE_DIR}
)

add_library(shared STATIC ${shared_STAT_SRCS})
