#
# NeoPZ adaptivity makefile
#
# It will be processed by GNU automake to produce Makefile.in
#
# $Id#
#

DISTCLEANFILES = @OOP_DISTCLEANFILES@
CLEANFILES     = @OOP_CLEANFILES@

-include $(top_srcdir)/@OOP_MK@

bin_PROGRAMS = main

main_SOURCES = main.cpp tsmalltask.cpp TParVector.cpp TParVector.h fluxdefs.h

main_LDADD = $(top_builddir)/lib/liboopar.a @MPILIBDIR@ @MPILIB@ -lpthread
## --| OOPar |------------------------------------------------------------------
noinst_HEADERS = tsmalltask.h
INCLUDES = -I$(top_srcdir)/Common -I$(top_srcdir)/Save -I$(top_srcdir)/Sources -I$(top_srcdir)/Util
AM_CXXFLAGS = @USING_MPI@
