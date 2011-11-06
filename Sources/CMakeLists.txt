# @file oopar/Sources/CMakeLists.txt  -- CMake file for the Integral module

file(GLOB headers *.h)
install(FILES ${headers} DESTINATION include/oopar)

file(GLOB sources *.cpp)

set(ooparsources_STAT_SRCS ${headers} ${sources} PARENT_SCOPE)


