#
# NeoPZ adaptivity makefile
#
# It will be processed by GNU automake to produce Makefile.in
#
# $Id#
#

DISTCLEANFILES = @OOP_DISTCLEANFILES@
CLEANFILES     = @OOP_CLEANFILES@

if OOPMPI
-include $(top_builddir)/@OOP_MK@
else
-include $(top_builddir)/@OOP_MK@
endif

bin_PROGRAMS = main

main_SOURCES = TTaskTest.cpp main.cpp

if OOPMPI
main_LDADD = $(top_builddir)/lib/liboopar.a @MPILIBDIR@ @MPILIB@ -lpthread
else
main_LDADD = -L$(top_builddir)/lib/ -loopar
endif

## --| OOPar |------------------------------------------------------------------
