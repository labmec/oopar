
file(GLOB header *.h)

set(testtask_SRCS main.cpp OOPMatVecMultiply.cpp OOPParMatIndexation.cpp OOPMergeMatrix.cpp OOPParMatrix.cpp )

add_executable(TestTask ${testtask_SRCS} ${header})

target_link_libraries(TestTask pz oopar mpi metis)

