project (src)

add_subdirectory(tcpip)
add_subdirectory(http)
add_subdirectory(example)

add_library(RESTClient SHARED $<TARGET_OBJECTS:tcpip> $<TARGET_OBJECTS:http>)
target_link_libraries(RESTClient ${Boost_IOSTREAMS_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${CPP} ${Boost_COROUTINE_LIBRARY} ${SUP_CPP})

if (ENABLE_TESTS)
  add_executable(test_cloud_files_upload test_cloud_files_upload.cpp)
  target_link_libraries(test_cloud_files_upload RESTClient)
  add_test(test_cloud_files_upload test_cloud_files_upload)

    add_executable(test_url_parser test_url_parser.cpp)
    target_link_libraries(test_url_parser RESTClient) 

endif()

install(FILES rest.hpp pimpl.hpp
        DESTINATION include/RESTClient)
install(TARGETS RESTClient LIBRARY DESTINATION lib)
