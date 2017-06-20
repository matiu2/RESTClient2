project(http)

add_library(http OBJECT url.cpp Request.cpp interface.cpp ReadResponse.cpp io_source_chunked.cpp)
#target_link_libraries(http ${Boost_IOSTREAMS_LIBRARY})

if (ENABLE_TESTS)
  add_executable(test_url test_url.cpp $<TARGET_OBJECTS:tcpip> $<TARGET_OBJECTS:http>)
  target_link_libraries(test_url ${Boost_IOSTREAMS_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${CPP} ${Boost_COROUTINE_LIBRARY} ${SUP_CPP})
  add_test(test_url test_url)

  add_executable(test_request test_request.cpp $<TARGET_OBJECTS:tcpip> $<TARGET_OBJECTS:http>)
  target_link_libraries(test_request ${Boost_IOSTREAMS_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${CPP} ${Boost_COROUTINE_LIBRARY} ${SUP_CPP})
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
            include/RESTClient/http)
