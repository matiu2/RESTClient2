project (src)

set(BUNCH_O_LIBS
    ${Boost_COROUTINE_LIBRARY} 
    ${Boost_CONTEXT_LIBRARY} 
    ${Boost_EXCEPTION_LIBRARY} 
    ${Boost_IOSTREAMS_LIBRARY} 
    ${Boost_THREAD_LIBRARY} 
    ${Boost_SYSTEM_LIBRARY} 
    ${OPENSSL_LIBRARIES} 
    ${CMAKE_THREAD_LIBS_INIT} 
    ${SUP_CPP} 
    ${z} 
    ${dl})

add_subdirectory(tcpip)
add_subdirectory(http)
add_subdirectory(example)

add_library(RESTClient STATIC $<TARGET_OBJECTS:tcpip> $<TARGET_OBJECTS:http>)
target_link_libraries(RESTClient ${BUNCH_O_LIBS})

if (ENABLE_TESTS)
  add_executable(test_cloud_files_upload test_cloud_files_upload.cpp)
  target_link_libraries(test_cloud_files_upload RESTClient)
  add_test(test_cloud_files_upload test_cloud_files_upload)

    add_executable(test_url_parser test_url_parser.cpp)
    target_link_libraries(test_url_parser RESTClient ${BUNCH_O_LIBS}) 

endif()

install(FILES rest.hpp pimpl.hpp
        DESTINATION include/RESTClient)
install(TARGETS RESTClient ARCHIVE DESTINATION lib)
