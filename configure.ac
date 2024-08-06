#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([klangc],[0.0.1],[mako10k@mk10.org])
AC_CONFIG_SRCDIR([src/klangc.c])
AC_CONFIG_HEADERS([config.h])

AC_USE_SYSTEM_EXTENSIONS

LT_INIT
AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES([GC], [bdw-gc])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile src/Makefile]) 

AC_CONFIG_MACRO_DIRS([m4])

AC_OUTPUT