dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(xcc, 0.1)

AM_INIT_AUTOMAKE()

AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB

m4_esyscmd([env top_srcdir=. src_dir_expr=\$srcdir sh ./generate_front_ends.sh])

AC_ARG_WITH([llvm-prefix],  [AS_HELP_STRING([--llvm-prefix],    [Where llvm is installed])],
        [llvm_prefix="$withval/"],
        [llvm_prefix="/usr"])

AC_SUBST([gtest_include],               [/home/derick/software/gtest/googletest/include])
AC_SUBST([gtest_libdir],                [/home/derick/software/gtest/googletest/lib])

AC_SUBST([llvm_config],                 [${llvm_prefix}/bin/llvm-config])
AC_SUBST([CXXFLAGS],                    ['-g2 -O0'])

AC_SUBST([llvm_version],                [`$llvm_config --version`])
AC_SUBST([llvm_version_major],          [${llvm_version:0:1}])
AC_SUBST([llvm_version_minor],          [${llvm_version:2:1}])
AC_SUBST([llvm_libs],                   [`$llvm_config --libs core engine scalaropts`])
AC_SUBST([llvm_syslibs],                [`$llvm_config --system-libs`])
AC_SUBST([llvm_cppflags],               [`$llvm_config --cppflags`])
AC_SUBST([llvm_cxxflags],               [`$llvm_config --cxxflags`])
AC_SUBST([llvm_libdir],                 [`$llvm_config --libdir`])
AC_SUBST([llvm_ldflags],                [`$llvm_config --ldflags`])

AC_ARG_ENABLE([dev-trace-managed-pointers], [], [], [])

AC_DEFINE_UNQUOTED([LLVM_MAJOR_VERSION], [${llvm_version_major}], [LLVM major version])
AC_DEFINE_UNQUOTED([LLVM_MINOR_VERSION], [${llvm_version_minor}], [LLVM minor version])

AS_IF([test "x$enable_dev_trace_managed_pointers" == xyes],
    [AC_DEFINE([DEV_TRACE_MANAGED_PTRS], [], [Enable development tracing of managed pointers])])

AC_CONFIG_HEADERS([xcc/config.h])

AC_CONFIG_FILES([Makefile xcc/Makefile] __xcc_lang_makefiles__)
AC_OUTPUT
