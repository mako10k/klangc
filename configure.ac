#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([klangc],[0.0.1],[mako10k@mk10.org])
AC_CONFIG_SRCDIR([src/klangc.c])
AC_CONFIG_HEADERS([config.h])

AC_USE_SYSTEM_EXTENSIONS

AM_INIT_AUTOMAKE([foreign subdir-objects])

# Checks for programs.
AC_PROG_CC
AC_PROG_YACC
if test "x$YACC" = "xbison -y"; then
    YACC="bison"
else
    printf "YACC: $YACC needs bison\n"
    exit 1
fi
AC_PROG_LEX([noyywrap])

# Checks for libraries.
PKG_CHECK_MODULES([GC], [bdw-gc])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile src/Makefile]) 

AC_ARG_ENABLE([debug],
  AS_HELP_STRING([--enable-debug], [Enable debugging]),
  [debug=$enableval], [debug=no])

if test "x$debug" = "xyes"; then
    CFLAGS="$CFLAGS -g -O0"
    AC_DEFINE([DEBUG], [1], [Define to 1 if debugging is enabled])
fi

AC_CONFIG_MACRO_DIRS([m4])

AC_OUTPUT