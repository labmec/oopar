#
# OOPar Sources makefile
#
# It will be processed by GNU automake to produce Makefile.in
#
# $Id#
#

DISTCLEANFILES = @OOP_DISTCLEANFILES@
CLEANFILES     = @OOP_CLEANFILES@

-include $(top_srcdir)/@OOP_MK@

oopardir = $(prefix)/Sources

oopar_HEADERS = $(wildcard *.h)

noinst_LIBRARIES = liboopar.a

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
	ooptaskcontrol.cpp


## --| OOPar |------------------------------------------------------------------
