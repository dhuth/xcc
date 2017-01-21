SUBDIRS=xpp xi ardvarc

bin_PROGRAMS      		= xcc

xcc_core_sources        = tree.cpp
xcc_core_cxxflags       = -std=c++11
xcc_core_libs           = -lLLVM-3.9
xcc_core_ldflags        = -L/usr/lib/llvm-3.9/lib

## Special source files
tree.cpp:				$(srcdir)/all_tree_types.def.hpp
tree.hpp:               $(srcdir)/all_tree_types.def.hpp

$(srcdir)/all_tree_types.def.hpp:
	$(srcdir)/generate_front_ends.sh


xcc_CXXFLAGS            = $(xcc_core_xccflags)
xcc_SOURCES             = $(xcc_coure_sources) xcc.cpp
xcc_LDADD               = $(xcc_core_libs)
xcc_LDFLAGS             = $(xcc_dore_ldflags)



check_PROGRAMS          = check_tree
TESTS                   = $(check_PROGRAMS)

check_libs              = $(xcc_core_libs) -lgtest
check_ldflags           = $(xcc_core_ldflags)
check_cxxflags          = $(xcc_core_cxxflags) -pthread

check_tree_SOURCES      = $(xcc_core_sources) check_tree.cpp
check_tree_CXXFLAGS     = $(check_cxxflags)
check_tree_LDADD        = $(check_libs)
check_tree_LDFLAGS      = $(check_ldflags)


