# output generic information about the core and buildtype chosen
message("")
message("* Project Secure Chat rev. hash  : ${rev_hash}")
message("* Project Secure Chat rev. date  : ${rev_date}")
if( UNIX )
  message("* Project Secure Chat buildtype  : ${CMAKE_BUILD_TYPE}")
endif()
message("")

# output information about installation-directories and locations

message("* Install core to        : ${CMAKE_INSTALL_PREFIX}")
if( UNIX )
  message("* Install libraries to   : ${LIBSDIR}")
  message("* Install configs to     : ${CONF_DIR}")
endif()
message("")

# Show infomation about the options selected during configuration

if( SERVER )
  message("* Build Server      : Yes (default)")
else()
  message("* Build Server : No")
endif()

if( CLIENT )
  message("* Build Client      : Yes (default)")
else()
  message("* Build Client : No")
endif()

if( WITH_WARNINGS )
  message("* Show all warnings      : Yes")
else()
  message("* Show compile-warnings  : No  (default)")
endif()

if( WITH_DEBUG )
  message("* Use coreside debug     : Yes")
  add_definitions(-D_DEBUG)
else()
  message("* Use coreside debug     : No  (default)")
endif()

message("")
