# @file oopar/lib/CMakeList.txt  -- CMake file grouping oopar modules and creating oopar library. 

#Creating groups for organized display of files into PZ modules
source_group(Socket FILES ${socket_STAT_SRCS})
source_group(Sources FILES ${sources_STAT_SRCS})
source_group(ParUtil FILES ${ooparutil_STAT_SRCS})

#creating pz lib and groups of the sources files
set(ooparsources ${socket_STAT_SRCS} ${ooparsources_STAT_SRCS} ${ooparutil_STAT_SRCS})

#creating target pz library and installing it
add_library(oopar ${ooparsources})

install(TARGETS oopar ARCHIVE DESTINATION lib)

