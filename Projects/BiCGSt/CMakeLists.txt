
file(GLOB header *.h)

set(bicgst_SRCS bicgdouble.cpp TDistNorm.cpp TDotProduct.cpp TLoopFor.cpp TMultAdd.cpp TUpdateResidue.cpp)

#add_executable(bicgst ${bicgst_SRCS} ${header})

#target_link_libraries(biggest pz oopar mpi)

