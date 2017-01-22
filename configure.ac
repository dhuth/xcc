dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(xcc, 0.1)

AM_INIT_AUTOMAKE()

AC_PROG_CC
AC_PROG_CXX

AC_CONFIG_HEADERS([xcc/config.h])

dnl TODO: automate from *.lang files
dnl         from m4_shell or something

AC_CONFIG_FILES([Makefile
                 xcc/Makefile
                 xcc/xi/Makefile
                 xcc/xpp/Makefile
                 xcc/ardvarc/Makefile])
AC_OUTPUT
