# @file oopar/socket/CMakeLists.txt  -- CMake file for the socket module

file(GLOB headers *.h)

add_executable(TestSocket_spd ${headers} spd.c)
target_link_libraries(TestSocket_spd pz mpi oopar)


