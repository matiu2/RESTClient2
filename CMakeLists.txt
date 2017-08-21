cmake_minimum_required(VERSION 3.7)

project(RESTClient)

option(IS_CLANG "Are we using the clang compiler?" ON)
option(ENABLE_TESTS "Enable building and running of tests?" ON)
option(LOG_THE_WIRE "Logs all data sent and receieved" OFF)
if (ENABLE_TESTS)
  enable_testing()
endif()
if (LOG_THE_WIRE)
  add_definitions(-DLOG_THE_WIRE)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -stdlib=libc++ -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -fstandalone-debug -fwritable-strings -fno-builtin")

include_directories("/usr/include/libcxxabi")

add_definitions(-DBOOST_COROUTINES_NO_DEPRECATION_WARNING)

# This is needed because boost asio still calls boost::coroutines 1, which is deprecated
add_definitions(-DBOOST_COROUTINE_NO_DEPRECATION_WARNING)

include(cmake/dependencies.cmake)

add_subdirectory(src)

