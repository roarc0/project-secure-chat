#if(CMAKE_COMPILER_IS_GNUCXX)
#  add_definitions(-fno-delete-null-pointer-checks)
#endif()

add_subdirectory(shared)

if( SERVER )  
  add_subdirectory(server)
endif()

if( CLIENT )
   add_subdirectory(client)
endif()

