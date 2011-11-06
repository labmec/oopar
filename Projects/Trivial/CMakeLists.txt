
file(GLOB header *.h)

set(trivial_SRCS ooparmain.cpp tmultidata.cpp tmultitask.cpp tresultdata.cpp TTaskTest.cpp )

add_executable(Trivial ${trivial_SRCS} ${header})

target_link_libraries(Trivial pz oopar mpi)

