bin_PROGRAMS = klangc

CFLAGS=-g -O0 -Wall -Werror -I$(top_srcdir)/src -I$(top_srcdir)/src/gc

klangc_SOURCES =         \
    expr/closure/bare.h  \
    expr/closure/bare.c  \
    expr/closure/bind.h  \
    expr/closure/bind.c  \
    expr/closure/entry.h \
    expr/closure/entry.c \
    expr/appl.h          \
    expr/appl.c          \
    expr/closure.h       \
    expr/closure.c       \
    expr/lambda.h        \
    expr/lambda.c        \
    expr/ref.h           \
    expr/ref.c           \
    pat/appl.h           \
    pat/appl.c           \
    pat/as.h             \
    pat/as.c             \
    pat/ref.h            \
    pat/ref.c            \
    expr.h               \
    expr.c               \
    hash.h               \
    hash.c               \
    input.h              \
    input.c              \
    klangc.h             \
    klangc.c             \
    malloc.h             \
    malloc.c             \
    output.h             \
    output.c             \
    parse.h              \
    parse.c              \
    pat.h                \
    pat.c                \
    ref.h                \
    ref.c                \
    symbol.h             \
    symbol.c             \
    types.h              \
    value.h              \
    value.c 

klangc_LDADD = @GC_LIBS@
klangc_CFLAGS = @GC_CFLAGS@
