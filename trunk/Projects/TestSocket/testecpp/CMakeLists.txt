# @file oopar/socket/CMakeLists.txt  -- CMake file for the socket module

file(GLOB headers *.h)

add_executable(TestSocket_cpp ${headers} testeS.cpp)
target_link_libraries(TestSocket_cpp pz oopar)


