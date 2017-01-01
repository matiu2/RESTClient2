project(http)

add_executable(test_url test_url.cpp url.cpp)
target_link_libraries(test_url ${CPP})
add_test(test_url test_url)
