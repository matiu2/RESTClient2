project(tcpip)

add_library(tcpip interface.cpp)
target_link_libraries(tcpip ${Boost_SYSTEM_LIBRARY} ${CPP})

