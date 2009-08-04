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
INCLUDES = $(all_includes) @LOG4CXX_INCLUDE@ $(PZ_INCLUDES) @MPIINCLUDE@ @MPEINCLUDE@

oopardir = $(prefix)/Sources $(prefix)/NeoPZ/Save

#oopar_HEADERS = $(wildcard *.h)
ooparincdir = $(includedir)/oopar
ooparinc_HEADERS= $(top_srcdir)/Sources/*.h

noinst_LIBRARIES = liboopar.a


#AM_CONDITIONAL(MPI, test "$enable_mpi" = yes)


if OOPMPI
MPI_SOURCES = \
	oopmpicomm.cpp \
	oopmpistorage.cpp
endif

if OOPSOCKET
SOCKET_SOURCES = oopsocket.cpp \
	oopsocketcommmanager.cpp \
	oopsocketstorage.cpp
endif

if OOPMPE
MPE_SOURCES = oopevtid.cpp \
	oopevtmanager.cpp
endif	

if OOPINTERNAL
INTERNAL_SOURCES = oopinternalcommanager.cpp \
	oopsocketstorage.cpp
endif

GENERAL_SOURCES = OOPDouble.cpp \
	oopaccesstag.cpp \
	oopaccesstaglist.cpp \
	oopaccesstagmultiset.cpp \
	oopcommmanager.cpp \
	oopdatamanager.cpp \
	oopdataversion.cpp \
	oopdelobjecttask.cpp \
	oopdmlock.cpp \
	OOPInt.cpp \
	oopmetadata.cpp \
	oopobjectid.cpp \
	ooppardefs.cpp \
	oopsnapshottask.cpp \
	oopstorage.cpp \
	ooptask.cpp \
	ooptaskcontrol.cpp \
	ooptaskmanager.cpp \
	ooptmlock.cpp \
	OOPVector.cpp \
	oopwaittask.cpp \
	oopcollector.cpp \
	oopaccesstag.cpp \
	oopaccesstaglist.cpp \
	oopaccesstagmultiset.cpp \
	oopcommmanager.cpp \
	oopdatamanager.cpp \
	oopdataversion.cpp \
	oopdelobjecttask.cpp \
	oopdmlock.cpp \
	OOPInt.cpp \
	oopmetadata.cpp \
	oopobjectid.cpp \
	ooppardefs.cpp \
	oopsnapshottask.cpp \
	oopstorage.cpp \
	ooptask.cpp \
	ooptaskcontrol.cpp \
	ooptaskmanager.cpp \
	ooptmlock.cpp \
	OOPVector.cpp \
	oopwaittask.cpp \
	oopcollector.cpp \
	oopterminationtask.cpp

liboopar_a_SOURCES = $(GENERAL_SOURCES) $(SOCKET_SOURCES) $(MPI_SOURCES) $(MPE_SOURCES) $(INTERNAL_SOURCES)
	
## --| OOPar |------------------------------------------------------------------

