bin_PROGRAMS = klangc

CFLAGS=-g -O0 -Wall -Werror -I$(top_srcdir)/src -I$(top_srcdir)/src/gc

klangc_SOURCES =   \
    bind.h         \
    bind.c         \
    env.h          \
    env.c          \
    expr.h         \
    expr.c         \
    expr_appl.h    \
    expr_appl.c    \
    expr_env.h     \
    expr_env.c     \
    expr_closure.h \
    expr_closure.c \
    expr_lambda.h  \
    expr_lambda.c  \
    expr_ref.h     \
    expr_ref.c     \
    hash.h         \
    hash.c         \
    input.h        \
    input.c        \
    klangc.h       \
    klangc.c       \
    malloc.h       \
    malloc.c       \
    output.h       \
    output.c       \
    parse.h        \
    parse.c        \
    pat.h          \
    pat.c          \
    pat_appl.h     \
    pat_appl.c     \
    pat_as.h       \
    pat_as.c       \
    pat_ref.h      \
    pat_ref.c      \
    ref.h          \
    ref.c          \
    ref_target.h   \
    ref_target.c   \
    symbol.h       \
    symbol.c       \
    types.h        \
    val.h          \
    val.c          \
    val_appl.h     \
    val_appl.c     \
    val_ref.h      \
    val_ref.c      

klangc_LDADD = @GC_LIBS@
klangc_CFLAGS = @GC_CFLAGS@
