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

main_SOURCES = main.cpp oopwaittask.cpp tsmalltask.cpp TParVector.cpp TParVector.h fluxdefs.h

main_LDADD = $(top_srcdir)/lib/liboopar.a $(top_srcdir)/lib/liboopar.a $(MPILIBDIR) $(MPILIB) -lpthread
## --| OOPar |------------------------------------------------------------------
noinst_HEADERS =  oopwaittask.h tsmalltask.h
