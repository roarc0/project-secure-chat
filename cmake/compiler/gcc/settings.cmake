# Set build-directive (used in core to tell which buildtype we used)
add_definitions(-D_BUILD_DIRECTIVE='"${CMAKE_BUILD_TYPE}"')

#add_definitions(-fno-delete-null-pointer-checks)

if( WITH_WARNINGS )
  add_definitions(-Wall -Wfatal-errors -Wextra)
  message(STATUS "GCC: All warnings enabled")
else()
  add_definitions(--no-warnings)
  message(STATUS "GCC: All warnings disabled")
endif()

if( WITH_DEBUG )
  add_definitions(-ggdb3)
  message(STATUS "GCC: Debug-flags set (-ggdb3)")
endif()
