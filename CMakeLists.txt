cmake_minimum_required(VERSION 3.7)

project(RESTClient)

option(IS_CLANG "Are we using the clang compiler?" ON)

if(${IS_CLANG})
    add_definitions(-fPIC -std=c++1z -stdlib=libc++)
endif()

include(cmake/dependencies.cmake)

add_subdirectory(src)
