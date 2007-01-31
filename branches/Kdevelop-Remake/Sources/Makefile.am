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


liboopar_a_SOURCES = oopaccesstag.cpp oopaccesstaglist.cpp oopdatamanager.cpp \
		ooptaskmanager.cpp oopdataversion.cpp ooperror.cpp oopmetadata.cpp oopobjectid.cpp \
		oopparclass.cpp ooptask.cpp ooptaskcontrol.cpp oopwaittask.cpp oopcommmanager.cpp \
		oopevtid.cpp oopevtmanager.cpp oopstorage.cpp oopstorage.h oopstore.cpp \
		ooppardefs.cpp ooppardefs.h oopmpicomm.cpp oopmpicomm.h oopmpistorage.cpp \
		oopmpistorage.h OOPInt.cpp OOPInt.h OOPVector.cpp OOPVector.h OOPDouble.cpp OOPDouble.h \
		oopsnapshottask.cpp oopaccesstagmultiset.cpp oopdmlock.cpp ooptmlock.cpp
	

## --| OOPar |------------------------------------------------------------------

noinst_HEADERS = oopsnapshottask.h oopaccesstag.h oopaccesstaglist.h \
		oopmpicomm.h oopmpistorage.h oopmpicomm.cpp oopmpistorage.cpp oopaccesstagmultiset.h \
		oopdmlock.h ooptmlock.h
