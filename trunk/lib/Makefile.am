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
liboopar_a_LIBADD = ../Sources/oopdatamanager.o ../Sources/ooptaskmanager.o \
	../Sources/oopdataversion.o ../Sources/ooperror.o ../Sources/oopmetadata.o ../Sources/oopobjectid.o \
	../Sources/oopparclass.o ../Sources/oopsaveable.o ../Sources/oopstorage.o ../Sources/oopstore.o \
	../Sources/ooptask.o ../Sources/oopmdatadepend.o ../Sources/ooptaskcontrol.o \
	../Sources/OOPDataLogger.o ../Sources/oopaccessinfo.o ../Sources/ooppardefs.o \
	../Sources/oopwaittask.o  ../Sources/oopmpicomm.o ../Sources/oopmpistorage.o \
  ../Sources/ooperror.o ../Sources/OOPInt.o ../Sources/OOPDouble.o
else 
liboopar_a_LIBADD =  ../Sources/oopdatamanager.o \
	../Sources/ooptaskmanager.o \
	../Sources/oopdataversion.o \
	../Sources/ooperror.o \
	../Sources/oopfilecomm.o \
	../Sources/oopfilestorage.o \
	../Sources/oopmetadata.o \
	../Sources/oopobjectid.o \
	../Sources/oopparclass.o \
	../Sources/oopsaveable.o \
	../Sources/oopstorage.o \
	../Sources/oopstore.o \
	../Sources/ooptask.o \
	../Sources/oopmdatadepend.o \
	../Sources/ooptaskcontrol.o \
	../Sources/OOPDataLogger.o \
	../Sources/oopaccessinfo.o \
	../Sources/ooppardefs.o \
	../Sources/oopwaittask.o

endif
## --| OOPar |------------------------------------------------------------------
