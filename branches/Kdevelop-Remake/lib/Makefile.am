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

liboopar_a_LIBADD = ../Sources/oopaccesstagmultiset.o \
	../Sources/oopaccesstaglist.o ../Sources/oopaccesstag.o ../Sources/ooptmlock.o ../Sources/oopdmlock.o \
	../Sources/oopaccesstaglist.o ../Sources/oopaccesstag.o ../Sources/oopdatamanager.o \
	../Sources/ooptaskmanager.o ../Sources/oopdataversion.o ../Sources/ooperror.o \
	../Sources/oopmetadata.o ../Sources/oopobjectid.o ../Sources/oopparclass.o \
	../Sources/oopstorage.o ../Sources/oopstore.o ../Sources/ooptask.o ../Sources/ooptaskcontrol.o \
	../Sources/ooppardefs.o ../Sources/oopwaittask.o ../Sources/oopmpicomm.o \
	../Sources/oopmpistorage.o ../Sources/ooperror.o ../Sources/OOPInt.o ../Sources/OOPDouble.o \
	../Sources/oopcommmanager.o ../Sources/OOPVector.o ../Sources/oopevtid.o ../Sources/oopevtmanager.o \
	../Sources/oopsnapshottask.o
## --| OOPar |------------------------------------------------------------------
