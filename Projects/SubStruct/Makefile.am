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

bin_PROGRAMS = substruct

substruct_SOURCES = OOPComputeMatricesTask.cpp	parsubstruct.cpp \
	OOPContribWeightTask.cpp \
	OOPContributeDiagonalTask.cpp	tpzdohrassembly.cpp \
	OOPDohrMatrix.cpp		tpzdohrmatrix.cpp \
	OOPDumbCommMan.cpp		tpzdohrprecond.cpp \
	OOPLinAlgTasks.cpp		tpzdohrsubstruct.cpp \
	TPZFParMatrix.cpp		tpzgensubstruct.cpp


##main_LDADD = $(top_builddir)/lib/liboopar.a @MPILIBDIR@ @MPILAMLIBDIR@ @PZ_LIBDIR@ @PZ_LIB@ @MPILIB@ @MPILAMLIB@ @MPELIB@ -lpthread
## --| OOPar |------------------------------------------------------------------
noinst_HEADERS = 
