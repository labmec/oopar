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
-include $(top_srcdir)/@OOP_MK@

oopardir = $(prefix)/Sources

oopar_HEADERS = $(wildcard *.h)

noinst_LIBRARIES = liboopar.a

#AM_CONDITIONAL(MPI, test "$enable_mpi" = yes)
if OOPMPI
liboopar_a_SOURCES = oopdatamanager.cpp \
	ooptaskmanager.cpp \
	oopdataversion.cpp \
	ooperror.cpp \
	oopfilecomm.cpp \
	oopfilestorage.cpp \
	oopmetadata.cpp \
	oopobjectid.cpp \
	oopparclass.cpp \
	oopsaveable.cpp \
	oopstorage.cpp \
	oopstore.cpp \
	ooptask.cpp \
	oopmdatadepend.cpp \
	ooptaskcontrol.cpp \
	oopaccessinfo.cpp \
	OOPDataLogger.cpp \
	oopmpicomm.cpp \
	oopmpistorage.cpp
else
liboopar_a_SOURCES = oopdatamanager.cpp \
	ooptaskmanager.cpp \
	oopdataversion.cpp \
	ooperror.cpp \
	oopfilecomm.cpp \
	oopfilestorage.cpp \
	oopmetadata.cpp \
	oopobjectid.cpp \
	oopparclass.cpp \
	oopsaveable.cpp \
	oopstorage.cpp \
	oopstore.cpp \
	ooptask.cpp \
	oopmdatadepend.cpp \
	ooptaskcontrol.cpp \
	oopaccessinfo.cpp \
	OOPDataLogger.cpp
endif

## --| OOPar |------------------------------------------------------------------
