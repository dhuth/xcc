noinst_LIBRARIES    = libxpp.a

libxpp_a_SOURCES    = parse.cpp
libxpp_a_SOURCES   += xpp.cpp

libxpp_a_CPPFLAGS   = @llvm_cppflags@
libxpp_a_CXXFLAGS   = -std=c++17 -pthread -fno-rtti

BUILT_SOURCES       = context.def.h
BUILT_SOURCES      += tokens.def.h

EXTRA_CLEAN         = context.def.h
EXTRA_CLEAN        += tokens.def.h

m4_cmd=m4 -P -I$(top_srcdir)/m4

context.def.h: $(srcdir)/context.def $(srcdir)/xppdef.m4
	$(m4_cmd) -Dswitch_context_def $(srcdir)/xppdef.m4 $(srcdir)/context.def > context.def.h

tokens.def.h:  $(srcdir)/tokens.def $(srcdir)/xppdef.m4
	$(m4_cmd) -Dswitch_token_def $(srcdir)/xppdef.m4 $(srcdir)/tokens.def > tokens.def.h

