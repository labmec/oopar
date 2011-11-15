# @file oopar/socket/CMakeLists.txt  -- CMake file for the socket module

file(GLOB headers *.h)

add_executable(TestSocket_cc ${headers} teste.c)
target_link_libraries(TestSocket_cc pz mpi oopar)


