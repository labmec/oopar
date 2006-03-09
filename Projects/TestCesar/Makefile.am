

-include @PZ_DIR@/PZ_user.mk
-include @OOP_DIR@/OOP.mk

INCLUDES = -I$(top_srcdir)/Sources @LOG4CXX_INCLUDE@ @MPIINCLUDE@ $(PZ_INCLUDES) $(OOP_INCLUDES)
METASOURCES = AUTO
bin_PROGRAMS = main
main_SOURCES = tmedonhotask.cpp tmedonhotask.h cesartest.cpp
main_LDADD = $(top_builddir)/lib/liboopar.a @PZ_LIBDIR@ @PZ_LIB@ -L/usr/local/lib @LOG4CXX_LIBDIR@ @LOG4CXX_LIB@ @MPILIBDIR@ @MPILIB@
