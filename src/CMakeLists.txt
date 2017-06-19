project (src)

add_executable(test_url_parser test_url_parser.cpp)
target_link_libraries(test_url_parser http)

add_subdirectory(tcpip)
add_subdirectory(http)
add_subdirectory(example)

add_library(RESTClient SHARED
            # Stuff to export from the http lib
            http/BodyDecoders.hpp
            http/Headers.hpp
            http/ReadResponse.hpp
            http/Request.hpp
            http/Response.hpp
            http/interface.hpp
            http/io_source_chunked.hpp
            http/io_source_tcpip.hpp
            http/sendBody.hpp
            http/url.hpp
            # Stuff to export from the tcpip lib
            tcpip/interface.hpp
            tcpip/Connection.hpp
            tcpip/Exceptions.hpp
            tcpip/SpyGuard.hpp
            tcpip/interface.hpp
)
target_link_libraries(RESTClient http tcpip)

if (ENABLE_TESTS)
  add_executable(test_cloud_files_upload test_cloud_files_upload.cpp)
  target_link_libraries(test_cloud_files_upload RESTClient)
  add_test(test_cloud_files_upload test_cloud_files_upload)
endif()

install(FILES rest.hpp pimpl.hpp
        DESTINATION include/RESTClient)
install(TARGETS RESTClient LIBRARY DESTINATION lib)
