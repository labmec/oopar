#
# OOPar lib makefile
#
# It will be processed by GNU automake to produce Makefile.in
#
# $Id#
#

DISTCLEANFILES = @OOP_DISTCLEANFILES@
CLEANFILES     = @OOP_CLEANFILES@

-include $(top_srcdir)/@OOP_MK@

lib_LIBRARIES = liboopar.a

liboopar_a_SOURCES =
if OOPMPI
OOPMPI_LIBADD = ../Sources/oopmpicomm.o ../Sources/oopmpistorage.o
endif 
if OOPSOCKET

OOPSOCKET_LIBADD = ../Sources/oopsocket.o \
	  ../Sources/oopsocketcommmanager.o \
	  ../Sources/oopsocketstorage.o 
endif

if OOPMPE
OOPMPE_LIBADD =	../Sources/oopevtid.o \
	  ../Sources/oopevtmanager.o
endif

if OOPINTERNAL
OOPINTERNAL_LIBADD = ../Sources/oopsocketstorage.o \
		../Sources/oopinternalcommanager.o
endif

GENERAL_OOP_LIBADD = ../Sources/OOPDouble.o \
	  ../Sources/oopaccesstag.o \
	  ../Sources/oopaccesstaglist.o \
	  ../Sources/oopaccesstagmultiset.o \
	  ../Sources/oopcommmanager.o \
	  ../Sources/oopdatamanager.o \
	  ../Sources/oopdataversion.o \
	  ../Sources/oopdelobjecttask.o \
	  ../Sources/OOPInt.o \
	  ../Sources/oopmetadata.o \
	  ../Sources/oopobjectid.o \
	  ../Sources/ooppardefs.o \
	  ../Sources/oopsnapshottask.o \
	  ../Sources/oopstorage.o \
	  ../Sources/ooptask.o \
	  ../Sources/ooptaskcontrol.o \
	  ../Sources/ooptaskmanager.o \
	  ../Sources/OOPVector.o \
	  ../Sources/oopwaittask.o \
	  ../Sources/oopcollector.o \
	  ../Sources/oopaccesstag.o \
	  ../Sources/oopaccesstaglist.o \
	  ../Sources/oopaccesstagmultiset.o \
	  ../Sources/oopcommmanager.o \
	  ../Sources/oopdatamanager.o \
	  ../Sources/oopdataversion.o \
	  ../Sources/oopdelobjecttask.o \
	  ../Sources/OOPInt.o \
	  ../Sources/oopmetadata.o \
	  ../Sources/oopobjectid.o \
	  ../Sources/ooppardefs.o \
	  ../Sources/oopsnapshottask.o \
	  ../Sources/oopstorage.o \
	  ../Sources/ooptask.o \
	  ../Sources/ooptaskcontrol.o \
	  ../Sources/ooptaskmanager.o \
	  ../Sources/OOPVector.o \
	  ../Sources/oopwaittask.o \
	  ../Sources/oopcollector.o \
	  ../Sources/oopterminationtask.o

liboopar_a_LIBADD = $(GENERAL_OOP_LIBADD) $(OOPSOCKET_LIBADD) $(OOPMPE_LIBADD) $(OOPMPI_LIBADD) $(OOPINTERNAL_LIBADD)

## --| OOPar |------------------------------------------------------------------
