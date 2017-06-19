cmake_minimum_required(VERSION 3.7)

project(RESTClient)

option(IS_CLANG "Are we using the clang compiler?" ON)
option(ENABLE_TESTS "Enable building and running of tests?" ON)
if (ENABLE_TESTS)
  enable_testing()
endif()
 
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fstandalone-debug -fwritable-strings -fno-builtin"
    CACHE STRING "Flags used by the compiler during debug builds." FORCE)

if(${IS_CLANG})
    add_definitions(-fPIC -std=c++1z -stdlib=libc++)
endif()

add_definitions(-DBOOST_COROUTINES_NO_DEPRECATION_WARNING)

# This is needed because boost asio still calls boost::coroutines 1, which is deprecated
add_definitions(-DBOOST_COROUTINE_NO_DEPRECATION_WARNING)

include(cmake/dependencies.cmake)

add_subdirectory(src)

