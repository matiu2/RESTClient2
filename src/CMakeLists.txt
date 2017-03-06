project (src)

add_executable(test_url_parser test_url_parser.cpp)
target_link_libraries(test_url_parser http)

add_subdirectory(tcpip)
add_subdirectory(http)
add_subdirectory(example)

if (ENABLE_TESTS)
  add_executable(test_cloud_files_upload test_cloud_files_upload.cpp)
  target_link_libraries(test_cloud_files_upload http)
  add_test(test_cloud_files_upload test_cloud_files_upload)
endif()

install(FILES rest.hpp url_parser.hpp
        DESTINATION include/RESTClient)
