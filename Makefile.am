#
# Top Makefile.am for OOPar
#
# It will be processed by GNU automake to produce Makefile.in
#
# $Id: Makefile.am,v 1.3 2004-05-03 14:14:17 phil Exp $
#

usermkdir = $(prefix)

usermk_DATA = OOP_user.mk

SUBDIRS = Sources lib Projects

DISTCLEANFILES = @OOP_DISTCLEANFILES@
CLEANFILES     = @OOP_CLEANFILES@

PACKAGE = OOP
DEVELOPER_ID = LabMeC

clean-project:
	cd Projects && make distclean

backup: clean-project distclean
	@ echo Making $(PACKAGE) backup...
	@ echo -----------------------------------------------------------------
	@ echo Generating tarball...
	@ cd .. && tar --exclude={*.a,*.so,*.bz2,.tm_*,.snprj} -cf \
          $(PACKAGE)-$(DEVELOPER_ID)-`date +%Y%m%d_%Hh%Mm`.tar ./Pantheon
	@ echo Compacting...
	@ cd .. && bzip2 $(PACKAGE)-*.tar
	@ echo Generated file:
	@ echo $(PACKAGE)-$(DEVELOPER_ID)-`date +%Y%m%d_%Hh%Mm`.tar
	@ echo -----------------------------------------------------------------

## --| OOPar |------------------------------------------------------------------
