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

main_SOURCES = ooparmain.cpp tmultidata.cpp tmultitask.cpp tresultdata.cpp TTaskTest.cpp

main_LDADD = $(top_srcdir)/lib/liboopar.a @MPIDIR@/lib/libmpi.a

## --| OOPar |------------------------------------------------------------------
