noinst_LIBRARIES    = libxi.a

libxi_a_SOURCES     = xi.cpp
libxi_a_SOURCES    += xi-parser.cpp
libxi_a_SOURCES    += xi-lex.cpp
libxi_a_SOURCES    += xi_builder.cpp
libxi_a_CPPFLAGS    = @llvm_cppflags@
libxi_a_CXXFLAGS    = -std=c++11 -pthread

BUILT_SOURCES       = $(srcdir)/xi-parser.hpp
BUILT_SOURCES      += $(srcdir)/xi-lex.hpp

$(srcdir)/xi-parser.hpp: xi-parser.cpp
$(srcdir)/xi-lex.hpp:    xi-lex.cpp

xi-parser.cpp: xi-dev.y
	bison -o xi-parser.cpp xi-dev.y

xi-lex.cpp: xi-dev.l
	flex -o xi-lex.cpp xi-dev.l

