
file(GLOB header *.h)

set(oopfiles_SRCS OOPComputeMatricesTask.cpp OOPContributeDiagonalTask.cpp OOPContribWeightTask.cpp OOPDumbCommMan.cpp
    TPZFParMatrix.cpp parsubstruct.cpp)


add_executable(SubStruct ${header} ${oopfiles_SRCS} substruct.cpp)

target_link_libraries(SubStruct pz oopar)

#add_executable(ParSubStruct ${header} ${oopfiles_SRCS} parsubstruct.cpp)
#TPZFParMatrix.cpp)

#target_link_libraries(ParSubStruct pz oopar)
