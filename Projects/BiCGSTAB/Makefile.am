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
#Include here all sources for the current project
main_SOURCES =

if OOPMPI
main_LDADD = $(top_srcdir)/lib/liboopar.a \
		-L$(top_srcdir)/lib/ \
		-loopar \
		-lpthread \
		$(MPILIBDIR) \
		$(MPILIB)
else
main_LDADD = $(top_srcdir)/lib/liboopar.a \
		-L$(top_srcdir)/lib/ \
		-loopar
endif
#@MPIDIR@/lib/libmpi.a

## --| OOPar |------------------------------------------------------------------
