
file(GLOB header *.h)

set(philflux_SRCS main.cpp tsmalltask.cpp TParVector.cpp )

add_executable(PhilFlux ${philflux_SRCS} ${header})

target_link_libraries(PhilFlux pz mpi oopar)

