project (example)

add_executable(example main.cpp $<TARGET_OBJECTS:tcpip>)
target_link_libraries(example ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${CPP} ${Boost_COROUTINE_LIBRARY} ${SUP_CPP})
