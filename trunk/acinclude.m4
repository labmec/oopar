dnl
dnl acinclude.m4 for OOPar
dnl
dnl Process this file with GNU aclocal to produce a configure script.
dnl
dnl $Id: acinclude.m4,v 1.1 2003-05-31 10:56:08 phil Exp $
dnl

dnl
dnl Greetings!
dnl
AC_DEFUN([OOP_GREETINGS],
[
    echo
    echo "+-----------------------------------------------+"
    echo "             Welcome to OOPar project"
    echo "+-----------------------------------------------+"
    echo
    echo "Configuring OOPar version:" $OOP_VERSION.$OOP_REV
    echo
])

dnl
dnl Checking g++ version
dnl
AC_DEFUN([OOP_PROG_CXX],
[
    AC_PROG_CXX
    case "$CXX" in
        c++ | g++)
           CXX_MAJOR=2
           CXX_MINOR=95
           AC_MSG_CHECKING(if $CXX version >= $CXX_MAJOR.$CXX_MINOR)
           AC_TRY_COMPILE([#include<features.h>],
             [
              #if !__GNUC_PREREQ($CXX_MAJOR, $CXX_MINOR)
              #error Bad version
              #endif
             ],
             AC_MSG_RESULT(ok),
             AC_MSG_ERROR($CXX invalid version! Must be >= $CXX_MAJOR.$CXX_MINOR))
             ;;
    esac
])

dnl
dnl Checking for ar
dnl
AC_DEFUN([OOP_PROG_AR],
[
    case "${AR-unset}" in
	unset) AC_CHECK_PROG(AR, ar, ar) ;;
	*) AC_CHECK_PROGS(AR, $AR ar, ar) ;;
    esac
    AC_SUBST(AR)
    AC_MSG_CHECKING(ar flags)
    case "${ARFLAGS-unset}" in
	unset) ARFLAGS="-rcsv" ;;
    esac
    AC_MSG_RESULT($ARFLAGS)
    AC_SUBST(ARFLAGS)
])

dnl
dnl Bye bye!
dnl
AC_DEFUN([OOP_BYEBYE],
[
    echo
    echo "Finished configuration for OOPar version" $OOP_VERSION.$OOP_REV
    echo

    echo "+-----------------------------------------------+"
    echo
    echo "   You hopefully configured OOPar project"
    echo
    echo "   Options:"
    echo

    case "${mpi_enabled}" in
      yes)
        echo "      -> MPI enabled."
      ;;
      no)
        echo "      -> MPI not enabled."
      ;;
    esac

    case "${sloan_enabled}" in
      yes)
        echo "      -> Sloan enabled."
      ;;
      no)
        echo "      -> Sloan not enabled."
      ;;
    esac

    case "${socket_enabled}" in
      yes)
        echo "      -> Socket enabled."
      ;;
      no)
        echo "      -> Socket not enabled."
      ;;
    esac


    echo
    echo "   type \"make\" to start compilation."
    echo "   type \"make install\" as root to install it."
    echo
    echo "+-----------------------------------------------+"
])

dnl --| OOPar |-----------------------------------------------------------------
