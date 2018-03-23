dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(xcc, 0.1)

AM_INIT_AUTOMAKE()

AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB

#TODO: define somewhere else

#                 #
# - M4 lib info - #
#                 #

AC_ARG_WITH([m4lib], [AS_HELP_STRING([--m4lib],     [Where m4lib is located (usually /usr/local/share/m4lib)])],
        [m4lib_dir="$withval/"],
        [m4lib_dir="/usr/local/share/m4lib"])

AC_DEFINE_UNQUOTED([M4LIB_DIR],                     ["${m4lib_dir}"],  [m4lib data directory])


#               #
# - LLVM Info - #
#               #

AC_ARG_WITH([llvm-prefix],  [AS_HELP_STRING([--llvm-prefix],    [Where llvm is installed])],
        [llvm_prefix="$withval/"],
        [llvm_prefix="/usr"])

AC_SUBST([llvm_config],                     [${llvm_prefix}/bin/llvm-config])

AC_SUBST([llvm_version],                    [`$llvm_config --version`])
AC_SUBST([llvm_version_major],              [${llvm_version:0:1}])
AC_SUBST([llvm_version_minor],              [${llvm_version:2:1}])
AC_SUBST([llvm_libs],                       [`$llvm_config --libs core engine scalaropts`])
AC_SUBST([llvm_syslibs],                    [`$llvm_config --system-libs`])
AC_SUBST([llvm_cppflags],                   [`$llvm_config --cppflags`])
AC_SUBST([llvm_cxxflags],                   [`$llvm_config --cxxflags`])
AC_SUBST([llvm_libdir],                     [`$llvm_config --libdir`])
AC_SUBST([llvm_ldflags],                    [`$llvm_config --ldflags`])

AC_DEFINE_UNQUOTED([LLVM_MAJOR_VERSION],    [${llvm_version_major}],            [LLVM major version])
AC_DEFINE_UNQUOTED([LLVM_MINOR_VERSION],    [${llvm_version_minor}],            [LLVM minor version])
AC_DEFINE_UNQUOTED([LLVM_EXEC_LLI],         ["${llvm_prefix}/bin/lli"],         [LLVM interpreter])
AC_DEFINE_UNQUOTED([LLVM_EXEC_LLC],         ["${llvm_prefix}/bin/llc"],         [LLVM compiler])
AC_DEFINE_UNQUOTED([LLVM_EXEC_AR],          ["${llvm_prefix}/bin/llvm-ar"],     [LLVM archiver])
AC_DEFINE_UNQUOTED([LLVM_EXEC_AS],          ["${llvm_prefix}/bin/llvm-as"],     [LLVM assembler])
AC_DEFINE_UNQUOTED([LLVM_EXEC_LINK],        ["${llvm_prefix}/bin/llvm-link"],   [LLVM linker])


#                        #
# - Debug & Trace Info - #
#                        #

AC_ARG_ENABLE([dev-trace-managed-pointers], [], [], [])
AC_ARG_ENABLE([debug],                      [], [], [])

AS_IF([test "x$enable_debug" == xyes],
    [AC_SUBST([CXXFLAGS],                        ['-g2 -O0'])],
    [AC_SUBST([CXXFLAGS],                        ['-O3'])])

AS_IF([test "x$enable_dev_trace_managed_pointers" == xyes],
    [AC_DEFINE([DEV_TRACE_MANAGED_PTRS], [], [Enable development tracing of managed pointers])])


m4_esyscmd([env m4lib_dir=/usr/local/share/m4lib top_srcdir=. sh ./generate_front_ends.sh])

AC_CONFIG_HEADERS([xcc/config.h])

AC_CONFIG_FILES([Makefile xcc/Makefile] __xcc_lang_makefiles__)
AC_OUTPUT
