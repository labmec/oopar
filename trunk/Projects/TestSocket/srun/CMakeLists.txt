# @file oopar/socket/CMakeLists.txt  -- CMake file for the socket module

file(GLOB headers *.h)

add_executable(TestSocket_srun ${headers} srun.c)
target_link_libraries(TestSocket_srun pz oopar)


