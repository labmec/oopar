CFLAGS	      = -g -pg -DDEBUG

CC	      = g++

DEST	      = .

EXTHDRS	      = ../gnu/gnudefs.h

HDRS	      = TTaskTest.h \
		cmdefs.h \
		oopcommmanager.h \
		oopdatamanager.h \
		oopdataversion.h \
		ooperror.h \
		oopfilecomm.h \
		oopfilestorage.h \
		oopmetadata.h \
		oopobjectid.h \
		ooppardefs.h \
		ooppartask.h \
		oopsaveable.h \
		oopstorage.h \
		ooptask.h \
		ooptaskmanager.h \
		tmultidata.h \
		tmultitask.h \
		tresultdata.h

INSTALL	      = cp

LD	      = $(CC)

LDFLAGS	      = -g -pg

LIBS	      =

MAKEFILE      = Makefile

OBJS	      = TTaskTest.o \
		ooparmain.o \
		oopdatamanager.o \
		oopdataversion.o \
		ooperror.o \
		oopfilecomm.o \
		oopfilestorage.o \
		oopmetadata.o \
		oopobjectid.o \
		oopparclass.o \
		oopsaveable.o \
		oopstorage.o \
		oopstore.o \
		ooptask.o \
		ooptaskmanager.o \
		tmultidata.o \
		tmultitask.o \
		tresultdata.o

PRINT	      = pr

PROGRAM       = ooparmain

SHELL	      = /bin/sh

SRCS	      = TTaskTest.c \
		ooparmain.c \
		oopdatamanager.c \
		oopdataversion.c \
		ooperror.c \
		oopfilecomm.c \
		oopfilestorage.c \
		oopmetadata.c \
		oopobjectid.c \
		oopparclass.c \
		oopsaveable.c \
		oopstorage.c \
		oopstore.c \
		ooptask.c \
		ooptaskmanager.c \
		tmultidata.c \
		tmultitask.c \
		tresultdata.c

SYSHDRS	      = /usr/include/_G_config.h \
		/usr/include/alloca.h \
		/usr/include/asm/errno.h \
		/usr/include/asm/sigcontext.h \
		/usr/include/bits/confname.h \
		/usr/include/bits/endian.h \
		/usr/include/bits/environments.h \
		/usr/include/bits/errno.h \
		/usr/include/bits/initspin.h \
		/usr/include/bits/libc-lock.h \
		/usr/include/bits/posix_opt.h \
		/usr/include/bits/pthreadtypes.h \
		/usr/include/bits/sched.h \
		/usr/include/bits/select.h \
		/usr/include/bits/sigaction.h \
		/usr/include/bits/sigcontext.h \
		/usr/include/bits/siginfo.h \
		/usr/include/bits/signum.h \
		/usr/include/bits/sigset.h \
		/usr/include/bits/sigstack.h \
		/usr/include/bits/sigthread.h \
		/usr/include/bits/stdio-lock.h \
		/usr/include/bits/stdio.h \
		/usr/include/bits/stdio_lim.h \
		/usr/include/bits/string.h \
		/usr/include/bits/string2.h \
		/usr/include/bits/sys_errlist.h \
		/usr/include/bits/time.h \
		/usr/include/bits/types.h \
		/usr/include/bits/waitflags.h \
		/usr/include/bits/waitstatus.h \
		/usr/include/bits/wchar.h \
		/usr/include/bits/wordsize.h \
		/usr/include/ctype.h \
		/usr/include/endian.h \
		/usr/include/errno.h \
		/usr/include/features.h \
		/usr/include/gconv.h \
		/usr/include/getopt.h \
		/usr/include/gnu/stubs.h \
		/usr/include/libio.h \
		/usr/include/linux/errno.h \
		/usr/include/pthread.h \
		/usr/include/sched.h \
		/usr/include/signal.h \
		/usr/include/stdio.h \
		/usr/include/stdlib.h \
		/usr/include/string.h \
		/usr/include/sys/cdefs.h \
		/usr/include/sys/select.h \
		/usr/include/sys/sysmacros.h \
		/usr/include/sys/types.h \
		/usr/include/sys/ucontext.h \
		/usr/include/time.h \
		/usr/include/ucontext.h \
		/usr/include/unistd.h \
		/usr/include/wchar.h \
		/usr/include/wctype.h \
		/usr/include/xlocale.h

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS) $(LIBS)
		@echo "Linking $(PROGRAM) ..."
		@$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)
		@echo "done"

clean:;		@rm -f $(OBJS) core

clobber:;	@rm -f $(OBJS) $(PROGRAM) core tags

depend:;	@mkmf -f $(MAKEFILE)

echo:;		@echo $(HDRS) $(SRCS)

index:;		@ctags -wx $(HDRS) $(SRCS)

install:	$(PROGRAM)
		@echo Installing $(PROGRAM) in $(DEST)
		@-strip $(PROGRAM)
		@if [ $(DEST) != . ]; then \
		(rm -f $(DEST)/$(PROGRAM); $(INSTALL) $(PROGRAM) $(DEST)); fi

print:;		@$(PRINT) $(HDRS) $(SRCS)

tags:           $(HDRS) $(SRCS); @ctags $(HDRS) $(SRCS)
###
TTaskTest.o: TTaskTest.h ooptask.h oopsaveable.h /usr/include/stdio.h \
	/usr/include/features.h /usr/include/sys/cdefs.h \
	/usr/include/gnu/stubs.h /usr/include/bits/types.h \
	/usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
	/usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
	/usr/include/bits/wchar.h /usr/include/wctype.h /usr/include/endian.h \
	/usr/include/bits/endian.h /usr/include/xlocale.h \
	/usr/include/gconv.h /usr/include/bits/stdio-lock.h \
	/usr/include/bits/libc-lock.h /usr/include/pthread.h \
	/usr/include/sched.h /usr/include/time.h /usr/include/bits/time.h \
	/usr/include/signal.h /usr/include/bits/sigset.h \
	/usr/include/bits/signum.h /usr/include/bits/siginfo.h \
	/usr/include/bits/wordsize.h /usr/include/bits/sigaction.h \
	/usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
	/usr/include/bits/sigstack.h /usr/include/ucontext.h \
	/usr/include/sys/ucontext.h /usr/include/bits/sigthread.h \
	/usr/include/bits/initspin.h /usr/include/bits/stdio_lim.h \
	/usr/include/bits/sys_errlist.h /usr/include/getopt.h \
	/usr/include/ctype.h /usr/include/bits/stdio.h oopdataversion.h \
	oopstorage.h /usr/include/stdlib.h /usr/include/bits/waitflags.h \
	/usr/include/bits/waitstatus.h /usr/include/sys/types.h \
	/usr/include/sys/select.h /usr/include/bits/select.h \
	/usr/include/sys/sysmacros.h /usr/include/alloca.h ooperror.h \
	/usr/include/string.h /usr/include/bits/string.h \
	/usr/include/bits/string2.h cmdefs.h ooppardefs.h oopobjectid.h \
	../gnu/gnudefs.h tmultidata.h oopmetadata.h oopdatamanager.h \
	tresultdata.h
ooparmain.o: /usr/include/stdlib.h /usr/include/features.h \
	/usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
	/usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h \
	/usr/include/endian.h /usr/include/bits/endian.h \
	/usr/include/xlocale.h /usr/include/sys/types.h \
	/usr/include/bits/types.h /usr/include/bits/pthreadtypes.h \
	/usr/include/bits/sched.h /usr/include/time.h \
	/usr/include/bits/time.h /usr/include/sys/select.h \
	/usr/include/bits/select.h /usr/include/bits/sigset.h \
	/usr/include/sys/sysmacros.h /usr/include/alloca.h \
	/usr/include/unistd.h /usr/include/bits/posix_opt.h \
	/usr/include/bits/environments.h /usr/include/bits/wordsize.h \
	/usr/include/bits/confname.h /usr/include/getopt.h \
	/usr/include/ctype.h oopstorage.h /usr/include/stdio.h \
	/usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
	/usr/include/bits/wchar.h /usr/include/wctype.h /usr/include/gconv.h \
	/usr/include/bits/stdio-lock.h /usr/include/bits/libc-lock.h \
	/usr/include/pthread.h /usr/include/sched.h /usr/include/signal.h \
	/usr/include/bits/signum.h /usr/include/bits/siginfo.h \
	/usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h \
	/usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h \
	/usr/include/ucontext.h /usr/include/sys/ucontext.h \
	/usr/include/bits/sigthread.h /usr/include/bits/initspin.h \
	/usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
	/usr/include/bits/stdio.h ooperror.h /usr/include/string.h \
	/usr/include/bits/string.h /usr/include/bits/string2.h oopfilecomm.h \
	oopcommmanager.h oopobjectid.h oopfilestorage.h oopdatamanager.h \
	ooppardefs.h ooptask.h oopsaveable.h oopdataversion.h cmdefs.h \
	oopmetadata.h ooptaskmanager.h ooppartask.h /usr/include/errno.h \
	/usr/include/bits/errno.h /usr/include/linux/errno.h \
	/usr/include/asm/errno.h tmultitask.h tmultidata.h tresultdata.h
oopdatamanager.o: oopdatamanager.h ooppardefs.h ooptask.h oopsaveable.h \
	/usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h \
	/usr/include/gnu/stubs.h /usr/include/bits/types.h \
	/usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
	/usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
	/usr/include/bits/wchar.h /usr/include/wctype.h /usr/include/endian.h \
	/usr/include/bits/endian.h /usr/include/xlocale.h \
	/usr/include/gconv.h /usr/include/bits/stdio-lock.h \
	/usr/include/bits/libc-lock.h /usr/include/pthread.h \
	/usr/include/sched.h /usr/include/time.h /usr/include/bits/time.h \
	/usr/include/signal.h /usr/include/bits/sigset.h \
	/usr/include/bits/signum.h /usr/include/bits/siginfo.h \
	/usr/include/bits/wordsize.h /usr/include/bits/sigaction.h \
	/usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
	/usr/include/bits/sigstack.h /usr/include/ucontext.h \
	/usr/include/sys/ucontext.h /usr/include/bits/sigthread.h \
	/usr/include/bits/initspin.h /usr/include/bits/stdio_lim.h \
	/usr/include/bits/sys_errlist.h /usr/include/getopt.h \
	/usr/include/ctype.h /usr/include/bits/stdio.h oopdataversion.h \
	oopstorage.h /usr/include/stdlib.h /usr/include/bits/waitflags.h \
	/usr/include/bits/waitstatus.h /usr/include/sys/types.h \
	/usr/include/sys/select.h /usr/include/bits/select.h \
	/usr/include/sys/sysmacros.h /usr/include/alloca.h ooperror.h \
	/usr/include/string.h /usr/include/bits/string.h \
	/usr/include/bits/string2.h cmdefs.h oopobjectid.h oopmetadata.h \
	ooptaskmanager.h oopcommmanager.h ../gnu/gnudefs.h tmultidata.h \
	tmultitask.h tresultdata.h
oopdataversion.o: oopdataversion.h oopstorage.h /usr/include/stdlib.h \
	/usr/include/features.h /usr/include/sys/cdefs.h \
	/usr/include/gnu/stubs.h /usr/include/bits/waitflags.h \
	/usr/include/bits/waitstatus.h /usr/include/endian.h \
	/usr/include/bits/endian.h /usr/include/xlocale.h \
	/usr/include/sys/types.h /usr/include/bits/types.h \
	/usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
	/usr/include/time.h /usr/include/bits/time.h \
	/usr/include/sys/select.h /usr/include/bits/select.h \
	/usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
	/usr/include/alloca.h /usr/include/stdio.h /usr/include/libio.h \
	/usr/include/_G_config.h /usr/include/wchar.h \
	/usr/include/bits/wchar.h /usr/include/wctype.h /usr/include/gconv.h \
	/usr/include/bits/stdio-lock.h /usr/include/bits/libc-lock.h \
	/usr/include/pthread.h /usr/include/sched.h /usr/include/signal.h \
	/usr/include/bits/signum.h /usr/include/bits/siginfo.h \
	/usr/include/bits/wordsize.h /usr/include/bits/sigaction.h \
	/usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
	/usr/include/bits/sigstack.h /usr/include/ucontext.h \
	/usr/include/sys/ucontext.h /usr/include/bits/sigthread.h \
	/usr/include/bits/initspin.h /usr/include/bits/stdio_lim.h \
	/usr/include/bits/sys_errlist.h /usr/include/getopt.h \
	/usr/include/ctype.h /usr/include/bits/stdio.h ooperror.h \
	/usr/include/string.h /usr/include/bits/string.h \
	/usr/include/bits/string2.h
ooperror.o: /usr/include/stdio.h /usr/include/features.h \
	/usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
	/usr/include/bits/types.h /usr/include/bits/pthreadtypes.h \
	/usr/include/bits/sched.h /usr/include/libio.h \
	/usr/include/_G_config.h /usr/include/wchar.h \
	/usr/include/bits/wchar.h /usr/include/wctype.h /usr/include/endian.h \
	/usr/include/bits/endian.h /usr/include/xlocale.h \
	/usr/include/gconv.h /usr/include/bits/stdio-lock.h \
	/usr/include/bits/libc-lock.h /usr/include/pthread.h \
	/usr/include/sched.h /usr/include/time.h /usr/include/bits/time.h \
	/usr/include/signal.h /usr/include/bits/sigset.h \
	/usr/include/bits/signum.h /usr/include/bits/siginfo.h \
	/usr/include/bits/wordsize.h /usr/include/bits/sigaction.h \
	/usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
	/usr/include/bits/sigstack.h /usr/include/ucontext.h \
	/usr/include/sys/ucontext.h /usr/include/bits/sigthread.h \
	/usr/include/bits/initspin.h /usr/include/bits/stdio_lim.h \
	/usr/include/bits/sys_errlist.h /usr/include/getopt.h \
	/usr/include/ctype.h /usr/include/bits/stdio.h ooperror.h \
	/usr/include/stdlib.h /usr/include/bits/waitflags.h \
	/usr/include/bits/waitstatus.h /usr/include/sys/types.h \
	/usr/include/sys/select.h /usr/include/bits/select.h \
	/usr/include/sys/sysmacros.h /usr/include/alloca.h \
	/usr/include/string.h /usr/include/bits/string.h \
	/usr/include/bits/string2.h
oopfilecomm.o: /usr/include/stdio.h /usr/include/features.h \
	/usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
	/usr/include/bits/types.h /usr/include/bits/pthreadtypes.h \
	/usr/include/bits/sched.h /usr/include/libio.h \
	/usr/include/_G_config.h /usr/include/wchar.h \
	/usr/include/bits/wchar.h /usr/include/wctype.h /usr/include/endian.h \
	/usr/include/bits/endian.h /usr/include/xlocale.h \
	/usr/include/gconv.h /usr/include/bits/stdio-lock.h \
	/usr/include/bits/libc-lock.h /usr/include/pthread.h \
	/usr/include/sched.h /usr/include/time.h /usr/include/bits/time.h \
	/usr/include/signal.h /usr/include/bits/sigset.h \
	/usr/include/bits/signum.h /usr/include/bits/siginfo.h \
	/usr/include/bits/wordsize.h /usr/include/bits/sigaction.h \
	/usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
	/usr/include/bits/sigstack.h /usr/include/ucontext.h \
	/usr/include/sys/ucontext.h /usr/include/bits/sigthread.h \
	/usr/include/bits/initspin.h /usr/include/bits/stdio_lim.h \
	/usr/include/bits/sys_errlist.h /usr/include/getopt.h \
	/usr/include/ctype.h /usr/include/bits/stdio.h /usr/include/stdlib.h \
	/usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h \
	/usr/include/sys/types.h /usr/include/sys/select.h \
	/usr/include/bits/select.h /usr/include/sys/sysmacros.h \
	/usr/include/alloca.h /usr/include/string.h \
	/usr/include/bits/string.h /usr/include/bits/string2.h \
	ooptaskmanager.h ooptask.h oopsaveable.h oopdataversion.h \
	oopstorage.h ooperror.h cmdefs.h ooppardefs.h oopobjectid.h \
	oopfilecomm.h oopcommmanager.h oopfilestorage.h ooppartask.h \
	oopdatamanager.h oopmetadata.h /usr/include/errno.h \
	/usr/include/bits/errno.h /usr/include/linux/errno.h \
	/usr/include/asm/errno.h
oopfilestorage.o: oopfilestorage.h /usr/include/stdlib.h \
	/usr/include/features.h /usr/include/sys/cdefs.h \
	/usr/include/gnu/stubs.h /usr/include/bits/waitflags.h \
	/usr/include/bits/waitstatus.h /usr/include/endian.h \
	/usr/include/bits/endian.h /usr/include/xlocale.h \
	/usr/include/sys/types.h /usr/include/bits/types.h \
	/usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
	/usr/include/time.h /usr/include/bits/time.h \
	/usr/include/sys/select.h /usr/include/bits/select.h \
	/usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
	/usr/include/alloca.h /usr/include/string.h \
	/usr/include/bits/string.h /usr/include/bits/string2.h \
	/usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
	/usr/include/wchar.h /usr/include/bits/wchar.h /usr/include/wctype.h \
	/usr/include/gconv.h /usr/include/bits/stdio-lock.h \
	/usr/include/bits/libc-lock.h /usr/include/pthread.h \
	/usr/include/sched.h /usr/include/signal.h /usr/include/bits/signum.h \
	/usr/include/bits/siginfo.h /usr/include/bits/wordsize.h \
	/usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h \
	/usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h \
	/usr/include/ucontext.h /usr/include/sys/ucontext.h \
	/usr/include/bits/sigthread.h /usr/include/bits/initspin.h \
	/usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
	/usr/include/getopt.h /usr/include/ctype.h /usr/include/bits/stdio.h \
	oopstorage.h ooperror.h oopsaveable.h oopdataversion.h cmdefs.h
oopmetadata.o: oopmetadata.h oopdataversion.h oopstorage.h \
	/usr/include/stdlib.h /usr/include/features.h \
	/usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
	/usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h \
	/usr/include/endian.h /usr/include/bits/endian.h \
	/usr/include/xlocale.h /usr/include/sys/types.h \
	/usr/include/bits/types.h /usr/include/bits/pthreadtypes.h \
	/usr/include/bits/sched.h /usr/include/time.h \
	/usr/include/bits/time.h /usr/include/sys/select.h \
	/usr/include/bits/select.h /usr/include/bits/sigset.h \
	/usr/include/sys/sysmacros.h /usr/include/alloca.h \
	/usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
	/usr/include/wchar.h /usr/include/bits/wchar.h /usr/include/wctype.h \
	/usr/include/gconv.h /usr/include/bits/stdio-lock.h \
	/usr/include/bits/libc-lock.h /usr/include/pthread.h \
	/usr/include/sched.h /usr/include/signal.h /usr/include/bits/signum.h \
	/usr/include/bits/siginfo.h /usr/include/bits/wordsize.h \
	/usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h \
	/usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h \
	/usr/include/ucontext.h /usr/include/sys/ucontext.h \
	/usr/include/bits/sigthread.h /usr/include/bits/initspin.h \
	/usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
	/usr/include/getopt.h /usr/include/ctype.h /usr/include/bits/stdio.h \
	ooperror.h /usr/include/string.h /usr/include/bits/string.h \
	/usr/include/bits/string2.h oopsaveable.h cmdefs.h oopdatamanager.h \
	ooppardefs.h ooptask.h oopobjectid.h oopcommmanager.h \
	ooptaskmanager.h
oopobjectid.o: oopobjectid.h oopstorage.h /usr/include/stdlib.h \
	/usr/include/features.h /usr/include/sys/cdefs.h \
	/usr/include/gnu/stubs.h /usr/include/bits/waitflags.h \
	/usr/include/bits/waitstatus.h /usr/include/endian.h \
	/usr/include/bits/endian.h /usr/include/xlocale.h \
	/usr/include/sys/types.h /usr/include/bits/types.h \
	/usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
	/usr/include/time.h /usr/include/bits/time.h \
	/usr/include/sys/select.h /usr/include/bits/select.h \
	/usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
	/usr/include/alloca.h /usr/include/stdio.h /usr/include/libio.h \
	/usr/include/_G_config.h /usr/include/wchar.h \
	/usr/include/bits/wchar.h /usr/include/wctype.h /usr/include/gconv.h \
	/usr/include/bits/stdio-lock.h /usr/include/bits/libc-lock.h \
	/usr/include/pthread.h /usr/include/sched.h /usr/include/signal.h \
	/usr/include/bits/signum.h /usr/include/bits/siginfo.h \
	/usr/include/bits/wordsize.h /usr/include/bits/sigaction.h \
	/usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
	/usr/include/bits/sigstack.h /usr/include/ucontext.h \
	/usr/include/sys/ucontext.h /usr/include/bits/sigthread.h \
	/usr/include/bits/initspin.h /usr/include/bits/stdio_lim.h \
	/usr/include/bits/sys_errlist.h /usr/include/getopt.h \
	/usr/include/ctype.h /usr/include/bits/stdio.h ooperror.h \
	/usr/include/string.h /usr/include/bits/string.h \
	/usr/include/bits/string2.h
oopparclass.o: ooptask.h oopsaveable.h /usr/include/stdio.h \
	/usr/include/features.h /usr/include/sys/cdefs.h \
	/usr/include/gnu/stubs.h /usr/include/bits/types.h \
	/usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
	/usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
	/usr/include/bits/wchar.h /usr/include/wctype.h /usr/include/endian.h \
	/usr/include/bits/endian.h /usr/include/xlocale.h \
	/usr/include/gconv.h /usr/include/bits/stdio-lock.h \
	/usr/include/bits/libc-lock.h /usr/include/pthread.h \
	/usr/include/sched.h /usr/include/time.h /usr/include/bits/time.h \
	/usr/include/signal.h /usr/include/bits/sigset.h \
	/usr/include/bits/signum.h /usr/include/bits/siginfo.h \
	/usr/include/bits/wordsize.h /usr/include/bits/sigaction.h \
	/usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
	/usr/include/bits/sigstack.h /usr/include/ucontext.h \
	/usr/include/sys/ucontext.h /usr/include/bits/sigthread.h \
	/usr/include/bits/initspin.h /usr/include/bits/stdio_lim.h \
	/usr/include/bits/sys_errlist.h /usr/include/getopt.h \
	/usr/include/ctype.h /usr/include/bits/stdio.h oopdataversion.h \
	oopstorage.h /usr/include/stdlib.h /usr/include/bits/waitflags.h \
	/usr/include/bits/waitstatus.h /usr/include/sys/types.h \
	/usr/include/sys/select.h /usr/include/bits/select.h \
	/usr/include/sys/sysmacros.h /usr/include/alloca.h ooperror.h \
	/usr/include/string.h /usr/include/bits/string.h \
	/usr/include/bits/string2.h cmdefs.h ooppardefs.h oopobjectid.h \
	ooptaskmanager.h oopdatamanager.h oopmetadata.h
oopsaveable.o: oopsaveable.h /usr/include/stdio.h /usr/include/features.h \
	/usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
	/usr/include/bits/types.h /usr/include/bits/pthreadtypes.h \
	/usr/include/bits/sched.h /usr/include/libio.h \
	/usr/include/_G_config.h /usr/include/wchar.h \
	/usr/include/bits/wchar.h /usr/include/wctype.h /usr/include/endian.h \
	/usr/include/bits/endian.h /usr/include/xlocale.h \
	/usr/include/gconv.h /usr/include/bits/stdio-lock.h \
	/usr/include/bits/libc-lock.h /usr/include/pthread.h \
	/usr/include/sched.h /usr/include/time.h /usr/include/bits/time.h \
	/usr/include/signal.h /usr/include/bits/sigset.h \
	/usr/include/bits/signum.h /usr/include/bits/siginfo.h \
	/usr/include/bits/wordsize.h /usr/include/bits/sigaction.h \
	/usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
	/usr/include/bits/sigstack.h /usr/include/ucontext.h \
	/usr/include/sys/ucontext.h /usr/include/bits/sigthread.h \
	/usr/include/bits/initspin.h /usr/include/bits/stdio_lim.h \
	/usr/include/bits/sys_errlist.h /usr/include/getopt.h \
	/usr/include/ctype.h /usr/include/bits/stdio.h oopdataversion.h \
	oopstorage.h /usr/include/stdlib.h /usr/include/bits/waitflags.h \
	/usr/include/bits/waitstatus.h /usr/include/sys/types.h \
	/usr/include/sys/select.h /usr/include/bits/select.h \
	/usr/include/sys/sysmacros.h /usr/include/alloca.h ooperror.h \
	/usr/include/string.h /usr/include/bits/string.h \
	/usr/include/bits/string2.h cmdefs.h
oopstorage.o: oopstorage.h /usr/include/stdlib.h /usr/include/features.h \
	/usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
	/usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h \
	/usr/include/endian.h /usr/include/bits/endian.h \
	/usr/include/xlocale.h /usr/include/sys/types.h \
	/usr/include/bits/types.h /usr/include/bits/pthreadtypes.h \
	/usr/include/bits/sched.h /usr/include/time.h \
	/usr/include/bits/time.h /usr/include/sys/select.h \
	/usr/include/bits/select.h /usr/include/bits/sigset.h \
	/usr/include/sys/sysmacros.h /usr/include/alloca.h \
	/usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
	/usr/include/wchar.h /usr/include/bits/wchar.h /usr/include/wctype.h \
	/usr/include/gconv.h /usr/include/bits/stdio-lock.h \
	/usr/include/bits/libc-lock.h /usr/include/pthread.h \
	/usr/include/sched.h /usr/include/signal.h /usr/include/bits/signum.h \
	/usr/include/bits/siginfo.h /usr/include/bits/wordsize.h \
	/usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h \
	/usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h \
	/usr/include/ucontext.h /usr/include/sys/ucontext.h \
	/usr/include/bits/sigthread.h /usr/include/bits/initspin.h \
	/usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
	/usr/include/getopt.h /usr/include/ctype.h /usr/include/bits/stdio.h \
	ooperror.h /usr/include/string.h /usr/include/bits/string.h \
	/usr/include/bits/string2.h cmdefs.h
oopstore.o: oopsaveable.h /usr/include/stdio.h /usr/include/features.h \
	/usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
	/usr/include/bits/types.h /usr/include/bits/pthreadtypes.h \
	/usr/include/bits/sched.h /usr/include/libio.h \
	/usr/include/_G_config.h /usr/include/wchar.h \
	/usr/include/bits/wchar.h /usr/include/wctype.h /usr/include/endian.h \
	/usr/include/bits/endian.h /usr/include/xlocale.h \
	/usr/include/gconv.h /usr/include/bits/stdio-lock.h \
	/usr/include/bits/libc-lock.h /usr/include/pthread.h \
	/usr/include/sched.h /usr/include/time.h /usr/include/bits/time.h \
	/usr/include/signal.h /usr/include/bits/sigset.h \
	/usr/include/bits/signum.h /usr/include/bits/siginfo.h \
	/usr/include/bits/wordsize.h /usr/include/bits/sigaction.h \
	/usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
	/usr/include/bits/sigstack.h /usr/include/ucontext.h \
	/usr/include/sys/ucontext.h /usr/include/bits/sigthread.h \
	/usr/include/bits/initspin.h /usr/include/bits/stdio_lim.h \
	/usr/include/bits/sys_errlist.h /usr/include/getopt.h \
	/usr/include/ctype.h /usr/include/bits/stdio.h oopdataversion.h \
	oopstorage.h /usr/include/stdlib.h /usr/include/bits/waitflags.h \
	/usr/include/bits/waitstatus.h /usr/include/sys/types.h \
	/usr/include/sys/select.h /usr/include/bits/select.h \
	/usr/include/sys/sysmacros.h /usr/include/alloca.h ooperror.h \
	/usr/include/string.h /usr/include/bits/string.h \
	/usr/include/bits/string2.h cmdefs.h
ooptask.o: ooptask.h oopsaveable.h /usr/include/stdio.h \
	/usr/include/features.h /usr/include/sys/cdefs.h \
	/usr/include/gnu/stubs.h /usr/include/bits/types.h \
	/usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
	/usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
	/usr/include/bits/wchar.h /usr/include/wctype.h /usr/include/endian.h \
	/usr/include/bits/endian.h /usr/include/xlocale.h \
	/usr/include/gconv.h /usr/include/bits/stdio-lock.h \
	/usr/include/bits/libc-lock.h /usr/include/pthread.h \
	/usr/include/sched.h /usr/include/time.h /usr/include/bits/time.h \
	/usr/include/signal.h /usr/include/bits/sigset.h \
	/usr/include/bits/signum.h /usr/include/bits/siginfo.h \
	/usr/include/bits/wordsize.h /usr/include/bits/sigaction.h \
	/usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
	/usr/include/bits/sigstack.h /usr/include/ucontext.h \
	/usr/include/sys/ucontext.h /usr/include/bits/sigthread.h \
	/usr/include/bits/initspin.h /usr/include/bits/stdio_lim.h \
	/usr/include/bits/sys_errlist.h /usr/include/getopt.h \
	/usr/include/ctype.h /usr/include/bits/stdio.h oopdataversion.h \
	oopstorage.h /usr/include/stdlib.h /usr/include/bits/waitflags.h \
	/usr/include/bits/waitstatus.h /usr/include/sys/types.h \
	/usr/include/sys/select.h /usr/include/bits/select.h \
	/usr/include/sys/sysmacros.h /usr/include/alloca.h ooperror.h \
	/usr/include/string.h /usr/include/bits/string.h \
	/usr/include/bits/string2.h cmdefs.h ooppardefs.h oopobjectid.h \
	ooptaskmanager.h oopdatamanager.h oopmetadata.h oopcommmanager.h \
	../gnu/gnudefs.h
ooptaskmanager.o: ooptaskmanager.h ooptask.h oopsaveable.h \
	/usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h \
	/usr/include/gnu/stubs.h /usr/include/bits/types.h \
	/usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
	/usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
	/usr/include/bits/wchar.h /usr/include/wctype.h /usr/include/endian.h \
	/usr/include/bits/endian.h /usr/include/xlocale.h \
	/usr/include/gconv.h /usr/include/bits/stdio-lock.h \
	/usr/include/bits/libc-lock.h /usr/include/pthread.h \
	/usr/include/sched.h /usr/include/time.h /usr/include/bits/time.h \
	/usr/include/signal.h /usr/include/bits/sigset.h \
	/usr/include/bits/signum.h /usr/include/bits/siginfo.h \
	/usr/include/bits/wordsize.h /usr/include/bits/sigaction.h \
	/usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
	/usr/include/bits/sigstack.h /usr/include/ucontext.h \
	/usr/include/sys/ucontext.h /usr/include/bits/sigthread.h \
	/usr/include/bits/initspin.h /usr/include/bits/stdio_lim.h \
	/usr/include/bits/sys_errlist.h /usr/include/getopt.h \
	/usr/include/ctype.h /usr/include/bits/stdio.h oopdataversion.h \
	oopstorage.h /usr/include/stdlib.h /usr/include/bits/waitflags.h \
	/usr/include/bits/waitstatus.h /usr/include/sys/types.h \
	/usr/include/sys/select.h /usr/include/bits/select.h \
	/usr/include/sys/sysmacros.h /usr/include/alloca.h ooperror.h \
	/usr/include/string.h /usr/include/bits/string.h \
	/usr/include/bits/string2.h cmdefs.h ooppardefs.h oopobjectid.h \
	oopcommmanager.h tmultidata.h oopmetadata.h oopdatamanager.h \
	tmultitask.h tresultdata.h
tmultidata.o: tmultidata.h oopmetadata.h oopdataversion.h oopstorage.h \
	/usr/include/stdlib.h /usr/include/features.h \
	/usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
	/usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h \
	/usr/include/endian.h /usr/include/bits/endian.h \
	/usr/include/xlocale.h /usr/include/sys/types.h \
	/usr/include/bits/types.h /usr/include/bits/pthreadtypes.h \
	/usr/include/bits/sched.h /usr/include/time.h \
	/usr/include/bits/time.h /usr/include/sys/select.h \
	/usr/include/bits/select.h /usr/include/bits/sigset.h \
	/usr/include/sys/sysmacros.h /usr/include/alloca.h \
	/usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
	/usr/include/wchar.h /usr/include/bits/wchar.h /usr/include/wctype.h \
	/usr/include/gconv.h /usr/include/bits/stdio-lock.h \
	/usr/include/bits/libc-lock.h /usr/include/pthread.h \
	/usr/include/sched.h /usr/include/signal.h /usr/include/bits/signum.h \
	/usr/include/bits/siginfo.h /usr/include/bits/wordsize.h \
	/usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h \
	/usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h \
	/usr/include/ucontext.h /usr/include/sys/ucontext.h \
	/usr/include/bits/sigthread.h /usr/include/bits/initspin.h \
	/usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
	/usr/include/getopt.h /usr/include/ctype.h /usr/include/bits/stdio.h \
	ooperror.h /usr/include/string.h /usr/include/bits/string.h \
	/usr/include/bits/string2.h oopsaveable.h cmdefs.h oopdatamanager.h \
	ooppardefs.h ooptask.h oopobjectid.h
tmultitask.o: tmultitask.h ooptask.h oopsaveable.h /usr/include/stdio.h \
	/usr/include/features.h /usr/include/sys/cdefs.h \
	/usr/include/gnu/stubs.h /usr/include/bits/types.h \
	/usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
	/usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
	/usr/include/bits/wchar.h /usr/include/wctype.h /usr/include/endian.h \
	/usr/include/bits/endian.h /usr/include/xlocale.h \
	/usr/include/gconv.h /usr/include/bits/stdio-lock.h \
	/usr/include/bits/libc-lock.h /usr/include/pthread.h \
	/usr/include/sched.h /usr/include/time.h /usr/include/bits/time.h \
	/usr/include/signal.h /usr/include/bits/sigset.h \
	/usr/include/bits/signum.h /usr/include/bits/siginfo.h \
	/usr/include/bits/wordsize.h /usr/include/bits/sigaction.h \
	/usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
	/usr/include/bits/sigstack.h /usr/include/ucontext.h \
	/usr/include/sys/ucontext.h /usr/include/bits/sigthread.h \
	/usr/include/bits/initspin.h /usr/include/bits/stdio_lim.h \
	/usr/include/bits/sys_errlist.h /usr/include/getopt.h \
	/usr/include/ctype.h /usr/include/bits/stdio.h oopdataversion.h \
	oopstorage.h /usr/include/stdlib.h /usr/include/bits/waitflags.h \
	/usr/include/bits/waitstatus.h /usr/include/sys/types.h \
	/usr/include/sys/select.h /usr/include/bits/select.h \
	/usr/include/sys/sysmacros.h /usr/include/alloca.h ooperror.h \
	/usr/include/string.h /usr/include/bits/string.h \
	/usr/include/bits/string2.h cmdefs.h ooppardefs.h oopobjectid.h \
	tmultidata.h oopmetadata.h oopdatamanager.h tresultdata.h \
	../gnu/gnudefs.h TTaskTest.h
tresultdata.o: tresultdata.h oopmetadata.h oopdataversion.h oopstorage.h \
	/usr/include/stdlib.h /usr/include/features.h \
	/usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
	/usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h \
	/usr/include/endian.h /usr/include/bits/endian.h \
	/usr/include/xlocale.h /usr/include/sys/types.h \
	/usr/include/bits/types.h /usr/include/bits/pthreadtypes.h \
	/usr/include/bits/sched.h /usr/include/time.h \
	/usr/include/bits/time.h /usr/include/sys/select.h \
	/usr/include/bits/select.h /usr/include/bits/sigset.h \
	/usr/include/sys/sysmacros.h /usr/include/alloca.h \
	/usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
	/usr/include/wchar.h /usr/include/bits/wchar.h /usr/include/wctype.h \
	/usr/include/gconv.h /usr/include/bits/stdio-lock.h \
	/usr/include/bits/libc-lock.h /usr/include/pthread.h \
	/usr/include/sched.h /usr/include/signal.h /usr/include/bits/signum.h \
	/usr/include/bits/siginfo.h /usr/include/bits/wordsize.h \
	/usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h \
	/usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h \
	/usr/include/ucontext.h /usr/include/sys/ucontext.h \
	/usr/include/bits/sigthread.h /usr/include/bits/initspin.h \
	/usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
	/usr/include/getopt.h /usr/include/ctype.h /usr/include/bits/stdio.h \
	ooperror.h /usr/include/string.h /usr/include/bits/string.h \
	/usr/include/bits/string2.h oopsaveable.h cmdefs.h oopdatamanager.h \
	ooppardefs.h ooptask.h oopobjectid.h
