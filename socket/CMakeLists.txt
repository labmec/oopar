# @file oopar/socket/CMakeLists.txt  -- CMake file for the socket module

file(GLOB headers *.h)
install(FILES ${headers} DESTINATION include/oopar)

set(socket_SRC_STAT ${headers} srun.c spd.c)

add_executable(srun ${headers} srun.c)
target_link_libraries(srun pz oopar mpi)

add_executable(spd ${headers} spd.c)
target_link_libraries(spd pz oopar mpi)


