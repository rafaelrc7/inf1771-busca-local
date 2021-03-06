#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([T1AI], [1.0], [contact@rafaelrc.com])
AC_CONFIG_SRCDIR([src/main.c])
AM_MAINTAINER_MODE
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AX_CHECK_COMPILE_FLAG([-std=gnu11], [], [
  echo "gnu11 standard is unsupported by compiler"
  exit -1
])

AC_PROG_MAKE_SET

# Checks for libraries.
PKG_CHECK_MODULES([LUA], [lua])
PKG_CHECK_MODULES([SDL2], [sdl2])
PKG_CHECK_MODULES([SDL2_TTF], [SDL2_ttf])

# Checks for header files.
AC_CHECK_HEADERS([stdint.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset srandom random strtol strtoul])
AX_GCC_BUILTIN(__builtin_ctz)
AX_GCC_BUILTIN(__builtin_clzl)

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

