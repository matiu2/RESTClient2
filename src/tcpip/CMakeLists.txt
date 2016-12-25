project(tcpip)

add_library(tcpip SHARED interface.cpp)
target_link_libraries(tcpip ${Boost_SYSTEM_LIBRARY} ${CPP} ${Boost_COROUTINE_LIBRARY})

add_executable(test_tcpip test_tcpip.cpp)
target_link_libraries(test_tcpip tcpip)
