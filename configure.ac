dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(xcc, 0.1)

AM_INIT_AUTOMAKE()

AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB

dnl TODO: automate from *.lang files
dnl         from m4_shell or something

dnl env top_srcdir=$srcdir ./generate_front_ends.sh > $srcdir/xcc_lang_libs

m4_esyscmd(env top_srcdir=. src_dir_expr=\$srcdir sh ./generate_front_ends.sh)

AC_SUBST([llvm_prefix],             [])
AC_SUBST([llvm_version],            [3.8])
AC_SUBST([llvm_version_expr],       [-${llvm_version}])
AC_SUBST([llvm_config],             [${llvm_prefix}llvm-config${llvm_version_expr}])
AC_SUBST([CXXFLAGS],                ['-g2 -O0'])

AC_SUBST([llvm_libs],               [`$llvm_config --libs core engine scalaropts`])
AC_SUBST([llvm_syslibs],            [`$llvm_config --system-libs`])
AC_SUBST([llvm_cppflags],           [`$llvm_config --cppflags`])
AC_SUBST([llvm_libdir],             [`$llvm_config --libdir`])
AC_SUBST([llvm_ldflags],            [`$llvm_config --ldflags`])

AC_CONFIG_HEADERS([xcc/config.h])

AC_CONFIG_FILES([Makefile xcc/Makefile] __xcc_lang_makefiles__)
AC_OUTPUT
