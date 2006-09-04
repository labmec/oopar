#
# NeoPZ adaptivity makefile
#
# It will be processed by GNU automake to produce Makefile.in
#
# $Id#
#

DISTCLEANFILES = @OOP_DISTCLEANFILES@
CLEANFILES     = @OOP_CLEANFILES@

-include $(top_builddir)/@OOP_MK@
bin_PROGRAMS = main
main_SOURCES = TTaskTest.cpp main.cpp
main_LDADD = $(top_builddir)/lib/liboopar.a @MPILIBDIR@ @MPILAMLIBDIR@ @PZ_LIBDIR@ @PZ_LIB@ @MPILIB@ @MPILAMLIB@ -lpthread

## --| OOPar |------------------------------------------------------------------
