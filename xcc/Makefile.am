SUBDIRS=@xcc_lang_dirs@

bin_PROGRAMS      		= xcc

xcc_core_sources        = frontend.cpp
xcc_core_sources       += ircodegen.cpp
xcc_core_sources       += managed_ptr.cpp
xcc_core_sources       += tree_base.cpp
xcc_core_sources       += cstr.cpp
xcc_core_sources       += ast.cpp
xcc_core_sources       += ast_builder.cpp
xcc_core_sources       += ast_builder_mangler.cpp
xcc_core_sources       += ast_context.cpp
xcc_core_sources	   += ast_eval.cpp
xcc_core_sources       += ast_metadata.cpp
xcc_core_sources       += ast_printer.cpp
xcc_core_sources       += ast_type.cpp
xcc_core_sources       += ast_type_func.cpp
xcc_core_sources       += ast_type_set.cpp
xcc_core_sources       += llvm_metadata.cpp
xcc_core_sources       += xcc.cpp

xcc_core_cppflags       = @llvm_cppflags@
xcc_core_cxxflags       = -std=c++17 -pthread -fno-rtti
xcc_core_libs           = @xcc_lang_libs@ @llvm_libs@ @llvm_syslibs@
xcc_core_ldflags        = @llvm_ldflags@

## Special source files
xcc.cpp:                $(srcdir)/all_frontends.def.hpp
tree_base.cpp:          $(srcdir)/all_tree_types.def.hpp
tree_base.hpp:          $(srcdir)/all_tree_types.def.hpp


xcc_CPPFLAGS            = $(xcc_core_cppflags)
xcc_CXXFLAGS            = $(xcc_core_cxxflags)
xcc_SOURCES             = $(xcc_core_sources)
xcc_LDADD               = $(xcc_core_libs)
xcc_LDFLAGS             = $(xcc_core_ldflags)

EXTRA_xcc_DEPENDENCIES  = @xcc_lang_libs@


