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
#/home/pos/longhin/Projetos/NeoPZ/Util/ /home/pos/longhin/Projetos/NeoPZ/

bin_PROGRAMS = main

main_SOURCES = TTaskComm.cpp TLocalCompute.cpp TContribution.cpp TParAnalysis.cpp TParCompute.cpp \
	TParMesh.cpp TParVector.cpp TPartitionRelation.cpp ooparmain.cpp multimain.cpp

main_LDADD = $(top_srcdir)/lib/liboopar.a \
		-L$(top_srcdir)/lib/ \
		-loopar
#@MPIDIR@/lib/libmpi.a

## --| OOPar |------------------------------------------------------------------
