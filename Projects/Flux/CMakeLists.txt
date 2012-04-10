
file(GLOB header *.h)

set(flux_SRCS TTaskComm.cpp TLocalCompute.cpp TContribution.cpp TParAnalysis.cpp TParCompute.cpp TParMesh.cpp TPartitionRelation.cpp ooparmain.cpp multimain.cpp
)

add_executable(Flux ${flux_SRCS} ${header})

target_link_libraries(Flux pz oopar)

