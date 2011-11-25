# @file oopar/CMakeList.txt  -- First file to read for CMake 

cmake_minimum_required (VERSION 2.8.0)

project(OOPAR)

#Specify where the headers files must be finded
include_directories(
	"${PROJECT_SOURCE_DIR}/Sources"
	"${PROJECT_SOURCE_DIR}/ParUtil"
 )

file(GLOB_RECURSE doxyfiles Documentation/*.h ./dox*.h)

#creating text files group 
add_custom_target(Doxygen_Files SOURCES  ${doxyfiles})

#Defining variables
#To cmake
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG" CACHE STRING "c++ debug flags go here" FORCE)
#To search files and libraries
	set(SEARCH_DIRS ENV PATH /usr/include /usr/local/include /usr/local/include/boost /usr/local/include/log4cxx /usr/lib /usr/local/lib ./. ./.. ./../libs ./../libs/fad ./../libs/metis ./../libs/metis/include ./../libs/metis/lib ./../libs/metis-5.0.1 ./../libs/metis-5.0.1/include ./../libs/metis-5.0.1/lib ./../externallibs ./../externallibs/fad ./../externallibs/metis ./../externallibs/metis/include ./../externallibs/metis/lib ./../externallibs/metis-5.0.1 ./../externallibs/metis-4.0 ./../externallibs/metis-5.0.1/include ./../externallibs/metis-4.0/include ./../externallibs/metis-5.0.1/lib ./../externallibs/neopz ./../externallibs/neopz/include ./../externallibs/neopz/lib)

#To install oopar
	set(OOPAR_INCLUDE ${PROJECT_SOURCE_DIR}/../externallibs/oopar/include)
	set(OOPAR_LIB ${PROJECT_SOURCE_DIR}/../externallibs/oopar/lib)

#ENABLING EXTERNAL LIBRARIES

#enabling pz library
find_path(NEOPZ_INCLUDE pzcreateapproxspace.h PATHS ${SEARCH_DIRS})
find_path(NEOPZ_LIB libpz.a PATHS ${SEARCH_DIRS})
if(NEOPZ_INCLUDE-NOTFOUND)
	set(NEOPZ_INCLUDE "/usr/local/include" CACHE PATH "Directory where neopz headers can be found")
endif()
if(NEOPZ_LIB-NOTFOUND)
	set (NEOPZ_LIB "/usr/local/lib" CACHE PATH "Directory where the neopz (pz) library can be found")
endif()

#To include directories of the PZ library
include_directories(${NEOPZ_INCLUDE})
link_directories(${NEOPZ_LIB})

#enabling pthread library
find_path(PTHREAD_INCLUDE pthread.h PATHS ${SEARCH_DIRS})
find_path(PTHREAD_LIB NAMES libpthread.dylib pthread.lib libpthread.a PATHS ${SEARCH_DIRS})
if(PTHREAD_INCLUDE-NOTFOUND)
	set (PTHREAD_INCLUDE "/usr/include" CACHE PATH "Directory where pthread.h can be found")
endif()
if(PTHREAD_LIB-NOTFOUND)
	set (PTHREAD_LIB "/usr/lib" CACHE PATH "Directory where the pthread library can be found")
endif()
include_directories(${PTHREAD_INCLUDE})
link_directories(${PTHREAD_LIB})
link_libraries(pthread)

#enabling Boost library 
option(USING_BOOST "Whether the Boost package will be linked with the library" ON)
if(USING_BOOST)
	find_package( Boost )
   	if(Boost_FOUND)
		add_definitions(-DUSING_BOOST)
      		include_directories(${Boost_INCLUDE_DIRS})
		link_directories(${Boost_LIB_DIRS})
		link_libraries(${Boost_LIB_DIRS})
   	endif()
endif(USING_BOOST)

#enabling to use LOG4CXX library
option(USING_LOG4CXX "Whether the LOG4CXX library will be linked in" OFF)
if(USING_LOG4CXX)
	find_path(LOG4CXX_INCLUDE log4cxx PATHS ${SEARCH_DIRS} ./../log4cxx ./../log4cxx/include)
	find_path(LOG4CXX_LIB NAMES liblog4cxx.dylib log4cxx.lib liblog4cxx.a PATHS ${SEARCH_DIRS} ./../log4cxx ./../log4cxx/lib)
	if(LOG4CXX_INCLUDE-NOTFOUND)
		set (LOG4CXX_INCLUDE "/usr/local/include" CACHE PATH "Directory where log4cxx header files can be found")
	else()
		add_definitions(-DLOG4CXX -DPZSOURCEDIR=\"${PROJECT_SOURCE_DIR}\")
		include_directories(${LOG4CXX_INCLUDE})
	endif()
	if(LOG4CXX_LIB-NOTFOUND)
		set (LOG4CXX_LIB "/usr/local/lib" CACHE PATH "Directory where the log4cxx library can be found")
	else()
		link_directories(${LOG4CXX_LIB})
		link_libraries(log4cxx)
	endif()
endif(USING_LOG4CXX)

#enabling METIS library
option(USING_METIS "Whether the METIS library will be linked in" OFF)
if(USING_METIS)
	find_path(METIS_INCLUDE metis.h PATHS ${SEARCH_DIRS} ./../metis-5.0.1 ./../metis-5.0.1/include)
	find_path(METIS_LIB NAMES libmetis.dylib metis.lib libmetis.a PATHS ${SEARCH_DIRS} ./../metis-5.0.1/libmetis ./../metis-5.0.1/lib)
	if(METIS_INCLUDE-NOTFOUND)
		set (METIS_INCLUDE "/usr/local/include" CACHE PATH "Directory where metis header files can be found")
	endif()
	if(METIS_LIB-NOTFOUND)
		set (METIS_LIB "/usr/local/lib" CACHE PATH "Directory where the metis library can be found")
	endif()
	#verifying whether the metis is metis-5
	string(FIND ${METIS_INCLUDE} "metis-5" METIS5)
	add_definitions(-DUSING_METIS)
	if(${METIS5} GREATER -1)
		add_definitions(-DMETIS_5)
	endif()
	include_directories(${METIS_INCLUDE})
	link_directories(${METIS_LIB})
	link_libraries(metis)
endif(USING_METIS)

#parallelism options
option(USING_SOCKET "To use Sockets to communication between parallel process" OFF)
option(USING_MPI "To use MPI to communication between parallel process" OFF)
option(USING_OOP_INTERNAL "To use internal protocol to communication between parallel process" OFF)
if(USING_SOCKET)
	add_definitions(-DOOP_SOCKET)
	set(USING_MPI OFF)
	set(USING_OOP_INTERNAL OFF)
endif(USING_SOCKET)
if(USING_MPI)
#Exist several linker erros about mpi. See: http://www.cmake.org/Wiki/CMake_2.8.4_Docs
	add_definitions(-DOOP_MPI)
	set(USING_SOCKET OFF)
	set(USING_OOP_INTERNAL OFF)
	#Enable MPI options
	find_package(MPI)
endif(USING_MPI)
if(USING_OOP_INTERNAL)
	add_definitions(-DOOP_INTERNAL)
	set(USING_SOCKET OFF)
	set(USING_MPI OFF)
endif(USING_OOP_INTERNAL)

#DEFINITIONS for Communications type
add_definitions(-DFILE_COMM)
add_definitions(-DBLOCKING)


#increment all the directories where the sources files is
add_subdirectory(Sources)
add_subdirectory(ParUtil)
add_subdirectory(lib)
add_subdirectory(Projects)

