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

main_SOURCES = main.cpp

main_LDADD = $(top_srcdir)/lib/liboopar.a ../Flux/TPartitionRelation.o ../Flux/TContribution.o $(top_srcdir)/lib/liboopar.a 
## --| OOPar |------------------------------------------------------------------
