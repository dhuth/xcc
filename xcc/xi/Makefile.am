subdirs=check

noinst_LIBRARIES    = libxi.a

libxi_a_SOURCES     = xi.cpp
libxi_a_SOURCES    += xi_builder.cpp
libxi_a_SOURCES    += xi_context.cpp
libxi_a_SOURCES    += xi_decl.cpp
libxi_a_SOURCES    += xi_dom.cpp
libxi_a_SOURCES    += xi_lookup.cpp
libxi_a_SOURCES    += xi_lower.cpp
libxi_a_SOURCES    += xi_metadata.cpp
libxi_a_SOURCES    += xi_name_mangler.cpp
libxi_a_SOURCES    += xi_semantic.cpp
libxi_a_SOURCES    += xi_struct.cpp
libxi_a_SOURCES    += xi_tc_id.cpp
libxi_a_SOURCES    += xi_tc_invoke.cpp
libxi_a_SOURCES    += xi_tc_op.cpp
libxi_a_SOURCES	   += xi_tc_stmt.cpp
libxi_a_SOURCES    += xi_tree.cpp
libxi_a_SOURCES    += xi_type.cpp
libxi_a_SOURCES    += xi_type_func.cpp
libxi_a_SOURCES    += xi_walker.cpp
libxi_a_SOURCES    += xi-parser.cpp
libxi_a_SOURCES    += xi-lex.cpp

libxi_a_CPPFLAGS    = @llvm_cppflags@
libxi_a_CXXFLAGS    = -std=c++17 -pthread -fno-rtti

BUILT_SOURCES       = xi-parser.hpp
BUILT_SOURCES      += xi-lex.hpp

EXTRA_CLEAN         = xi-parser.hpp
EXTRA_CLEAN        += xi-parser.cpp
EXTRA_CLEAN        += xi-lex.hpp
EXTRA_CLEAN        += xi-lex.cpp

xi-parser.hpp: xi-parser.cpp
xi-lex.hpp:    xi-lex.cpp

xi-parser.cpp: $(srcdir)/xi-dev.y
	bison -o xi-parser.cpp --report-file=xi-dev.y.log --report=all $(srcdir)/xi-dev.y

xi-lex.cpp: $(srcdir)/xi-dev.l
	flex -o xi-lex.cpp $(srcdir)/xi-dev.l

TESTS=@xi_lang_tests@
TEST_EXTENSIONS=.sh
