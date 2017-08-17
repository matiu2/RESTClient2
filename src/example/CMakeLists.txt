project (example)

add_executable(example main.cpp $<TARGET_OBJECTS:tcpip>)
target_link_libraries(example ${BUNCH_O_LIBS})
