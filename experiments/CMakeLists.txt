project(experiments)

add_executable(zlib zlib.cpp)
target_link_libraries(zlib http)
