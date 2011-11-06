
file(GLOB header *.h)

set(testcesar_SRCS tmedonhotask.cpp cesartest.cpp)

add_executable(TestCesar ${testcesar_SRCS} ${header})

target_link_libraries(TestCesar pz oopar mpi)

