project (src)

add_executable(test_url_parser test_url_parser.cpp url_parser.cpp)
target_link_libraries(test_url_parser ${Boost_SYSTEM_LIBRARY})

add_subdirectory(eventloop)
add_subdirectory(http)
add_subdirectory(tcpip)
add_subdirectory(example)
