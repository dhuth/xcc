SUBDIRS=xpp xi ardvarc

bin_PROGRAMS      		= xcc

xcc_core_sources        = frontend.cpp
xcc_core_sources       += ircodegen.cpp
xcc_core_sources       += managed_ptr.cpp
xcc_core_sources       += tree.cpp
xcc_core_sources       += ast_builder.cpp

xcc_core_cppflags       = -I@llvm_includedir@
xcc_core_cxxflags       = @llvm_cxxflags@ -fexceptions -std=c++11 -Wno-switch -Wno-unused-variable
xcc_core_libs           = @llvm_libs@ xi/libxi.a
xcc_core_ldflags        = -L@llvm_libdir@

## Special source files
xcc.cpp:                $(srcdir)/all_frontends.def.hpp
tree.cpp:               $(srcdir)/all_tree_types.def.hpp
tree.hpp:               $(srcdir)/all_tree_types.def.hpp


xcc_CPPFLAGS            = $(xcc_core_cppflags)
xcc_CXXFLAGS            = $(xcc_core_cxxflags)
xcc_SOURCES             = $(xcc_core_sources) xcc.cpp
xcc_LDADD               = $(xcc_core_libs)
xcc_LDFLAGS             = $(xcc_core_ldflags)



check_PROGRAMS          = check_tree
check_PROGRAMS         += check_ast
TESTS                   = $(check_PROGRAMS)

check_libs              = $(xcc_core_libs) -lgtest
check_ldflags           = $(xcc_core_ldflags)
check_cxxflags          = $(xcc_core_cxxflags) -pthread

check_tree_SOURCES      = check_tree.cpp $(xcc_core_sources)
check_tree_CXXFLAGS     = $(check_cxxflags)
check_tree_LDADD        = $(check_libs)
check_tree_LDFLAGS      = $(check_ldflags)

check_ast_SOURCES       = check_ast.cpp $(xcc_core_sources)
check_ast_CXXFLAGS      = $(check_cxxflags)
check_ast_LDADD         = $(check_libs)
check_ast_LDFLAGS       = $(check_ldflags)


