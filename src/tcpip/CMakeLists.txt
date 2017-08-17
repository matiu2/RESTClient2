project(tcpip)

add_library(tcpip OBJECT interface.cpp Connection.cpp ../logging.cpp)
#target_link_libraries(tcpip ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${CPP} ${Boost_COROUTINE_LIBRARY} ${SUP_CPP})

if (ENABLE_TESTS)
  add_executable(test_tcpip test_tcpip.cpp $<TARGET_OBJECTS:tcpip>)
  target_link_libraries(test_tcpip ${BUNCH_O_LIBS})
  add_test(test_tcpip test_tcpip)
endif()

install(FILES 
            Connection.hpp
            Exceptions.hpp
            SpyGuard.hpp
            interface.hpp
        DESTINATION
            include/RESTClient/tcpip)
