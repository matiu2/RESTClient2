project(http)

add_library(http SHARED url.cpp Request.cpp interface.cpp ReadResponse.cpp interface.cpp io_source_chunked.cpp)


target_link_libraries(http tcpip ${CPP} ${Boost_SYSTEM_LIBRARY} ${Boost_COROUTINE_LIBRARY} ${Boost_IOSTREAMS_LIBRARY} ${OPENSSL_LIBRARIES})

if (ENABLE_TESTS)
  add_executable(test_url test_url.cpp)
  target_link_libraries(test_url http)
  add_test(test_url test_url)

  add_executable(test_request test_request.cpp)
  target_link_libraries(test_request http)
  add_test(test_request test_request)
endif()

install(FILES 
            BodyDecoders.hpp
            Headers.hpp
            ReadResponse.hpp
            Request.hpp
            Response.hpp
            interface.hpp
            io_source_chunked.hpp
            io_source_tcpip.hpp
            sendBody.hpp
            url.hpp
        DESTINATION
            include/RESTClient/tcpip)
