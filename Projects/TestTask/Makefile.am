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
-include $(top_srcdir)/@OOP_MK@ $(mpi_dir)/include
#/home/pos/longhin/Projetos/NeoPZ/Util/ /home/pos/longhin/Projetos/NeoPZ/
else
-include $(top_srcdir)/@OOP_MK@
endif

bin_PROGRAMS = main

main_SOURCES = TTaskTest.cpp main.cpp

if OOPMPI
main_LDADD = $(top_builddir)/lib/liboopar.a -lpthread @MPILIBDIR@ @MPILIB@
else
main_LDADD = $(top_srcdir)/lib/liboopar.a \
		-L$(top_srcdir)/lib/ \
		-loopar
endif
#@MPIDIR@/lib/libmpi.a

## --| OOPar |------------------------------------------------------------------
INCLUDES = -I$(top_srcdir)/Common -I$(top_srcdir)/Save -I$(top_srcdir)/Sources -I$(top_srcdir)/Util
AM_CXXFLAGS = -DMPI
