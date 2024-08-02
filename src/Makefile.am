bin_PROGRAMS = klangc

CFLAGS=-g -O0 -Wall -Werror -I$(top_srcdir)/src -I$(top_srcdir)/src/gc

klangc_SOURCES =     \
    klangc_def.h     \
    klangc_def.c     \
    klangc_dict.h    \
    klangc_dict.c    \
    klangc_expr.h    \
    klangc_expr.c    \
    klangc_hash.h    \
    klangc_hash.c    \
    klangc_input.h   \
    klangc_input.c   \
    klangc_malloc.h  \
    klangc_malloc.c  \
    klangc_message.h \
    klangc_message.c \
    klangc_output.h  \
    klangc_output.c  \
    klangc_parse.h   \
    klangc_parse.c   \
    klangc_pattern.h \
    klangc_pattern.c \
    klangc_types.h   \
    klangc_value.h   \
    klangc_value.c   \
    klangc.h         \
    klangc.c

klangc_LDADD = @GC_LIBS@
klangc_CFLAGS = @GC_CFLAGS@
