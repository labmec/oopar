#
# OOPar Sources makefile
#
# It will be processed by GNU automake to produce Makefile.in
#
# $Id#
#

DISTCLEANFILES = @OOP_DISTCLEANFILES@
CLEANFILES     = @OOP_CLEANFILES@
#Acredito que aqui tb precisamos de um include de mpi
-include  $(top_builddir)/OOP.mk

# set the include path found by configure
INCLUDES = $(all_includes) @LOG4CXX_INCLUDE@ $(PZ_INCLUDES) @MPIINCLUDE@

oopardir = $(prefix)/Sources $(prefix)/NeoPZ/Save

#oopar_HEADERS = $(wildcard *.h)
ooparincdir = $(includedir)/oopar
ooparinc_HEADERS= $(top_srcdir)/Sources/*.h

noinst_LIBRARIES = liboopar.a


#AM_CONDITIONAL(MPI, test "$enable_mpi" = yes)
if OOPMPI
liboopar_a_SOURCES = oopdatamanager.cpp \
	ooptaskmanager.cpp 	\
	oopdataversion.cpp 	\
	ooperror.cpp \
	oopmetadata.cpp \
	oopobjectid.cpp \
	oopparclass.cpp \
	ooptask.cpp \
	oopmdatadepend.cpp \
	ooptaskcontrol.cpp \
	oopaccessinfo.cpp \
	OOPDataLogger.cpp \
	ooppardefs.cpp \
	oopmpicomm.cpp \
	oopmpistorage.cpp \
	oopwaittask.cpp \
	OOPInt.cpp \
	OOPDouble.cpp \
	OOPVector.cpp \
  oopstorage.cpp \
  oopstore.cpp \
  oopsaveable.cpp 
#  oopfilecomm.cpp 
#  oopfilestorage.cpp


#  ooppardefs.cpp
else
liboopar_a_SOURCES = oopdatamanager.cpp \
	ooptaskmanager.cpp \
	oopdataversion.cpp \
	ooperror.cpp \
	oopmetadata.cpp \
	oopobjectid.cpp \
	oopparclass.cpp \
	ooptask.cpp \
	oopmdatadepend.cpp \
	ooptaskcontrol.cpp \
	oopaccessinfo.cpp \
	OOPDataLogger.cpp \
	oopwaittask.cpp
endif

## --| OOPar |------------------------------------------------------------------

