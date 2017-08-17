project(http)

add_library(http OBJECT url.cpp Request.cpp interface.cpp ReadResponse.cpp io_source_chunked.cpp)
#target_link_libraries(http ${Boost_IOSTREAMS_LIBRARY})

if (ENABLE_TESTS)
  add_executable(test_url test_url.cpp $<TARGET_OBJECTS:tcpip> $<TARGET_OBJECTS:http>)
  target_link_libraries(test_url ${BUNCH_O_LIBS})
  add_test(test_url test_url)

  add_executable(test_request test_request.cpp $<TARGET_OBJECTS:tcpip> $<TARGET_OBJECTS:http>)
  target_link_libraries(test_request ${BUNCH_O_LIBS})
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
