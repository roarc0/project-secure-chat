file(GLOB_RECURSE sources_Channel channel/*.cpp channel/*.h)
file(GLOB_RECURSE sources_Database database/*.cpp database/*.h)
file(GLOB_RECURSE sources_Execution execution/*.cpp execution/*.h)
file(GLOB sources_Networking networking/*.cpp networking/*.h)
file(GLOB_RECURSE sources_Session session/*.cpp session/*.h)
file(GLOB_RECURSE sources_Threading threading/*.cpp threading/*.h)
file(GLOB sources_localdir *.cpp *.h)

set(server_SRCS
  ${sources_Channel}
  ${sources_Database}
  ${sources_Execution}
  ${sources_Networking}
  ${sources_Session}
  ${sources_Threading}
  ${sources_localdir}
)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/channel
  ${CMAKE_CURRENT_SOURCE_DIR}/database
  ${CMAKE_CURRENT_SOURCE_DIR}/execution
  ${CMAKE_CURRENT_SOURCE_DIR}/networking
  ${CMAKE_CURRENT_SOURCE_DIR}/session
  ${CMAKE_CURRENT_SOURCE_DIR}/threading
  ${OPENSSL_INCLUDE_DIR}
)

set(server_LINK_FLAGS "")

add_executable(server
  ${server_SRCS}
)

add_dependencies(server revision.h)

if( NOT WIN32 )
  add_definitions(-D_server_CONFIG="${CONF_DIR}/server.conf")
endif()

if( UNIX )
  set(server_LINK_FLAGS "-pthread -confuse -sqlite3 ${server_LINK_FLAGS}")
endif()

if( CMAKE_SYSTEM_NAME MATCHES "Darwin" )
  set(server_LINK_FLAGS "-framework Carbon ${server_LINK_FLAGS}")
endif()

set_target_properties(server PROPERTIES LINK_FLAGS "${server_LINK_FLAGS}")

target_link_libraries(server
  shared
  ${OPENSSL_LIBRARIES}
  ${OPENSSL_EXTRA_LIBRARIES}
  ${OSX_LIBS}
)

if( WIN32 )
  add_custom_command(TARGET server
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/server.conf.dist ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/
  )
endif()

if( UNIX )
  install(TARGETS server DESTINATION bin)
  install(FILES  server.conf.dist DESTINATION etc)
elseif( WIN32 )
  install(TARGETS server DESTINATION "${CMAKE_INSTALL_PREFIX}")
  install(FILES server.conf.dist DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()

