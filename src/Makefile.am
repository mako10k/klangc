bin_PROGRAMS = klangc

CFLAGS=-g -O0 -Wall -Werror -I$(top_srcdir)/src -I$(top_srcdir)/src/gc

klangc_SOURCES =     \
    klangc_bind.h    \
    klangc_bind.c    \
    klangc_closure.h \
    klangc_closure.c \
    klangc_eclosure.h\
    klangc_eclosure.c\
    klangc_elambda.h \
    klangc_elambda.c \
    klangc_expr.h    \
    klangc_expr.c    \
    klangc_hash.h    \
    klangc_hash.c    \
    klangc_input.h   \
    klangc_input.c   \
    klangc_malloc.h  \
    klangc_malloc.c  \
    klangc_output.h  \
    klangc_output.c  \
    klangc_parse.h   \
    klangc_parse.c   \
    klangc_pattern.h \
    klangc_pattern.c \
    klangc_ref.h     \
    klangc_ref.c     \
    klangc_symbol.h  \
    klangc_symbol.c  \
    klangc_types.h   \
    klangc_value.h   \
    klangc_value.c   \
    klangc.h         \
    klangc.c

klangc_LDADD = @GC_LIBS@
klangc_CFLAGS = @GC_CFLAGS@
