dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(hb, 1.0)


AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE()

AC_PROG_CXX

AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_LANG([C++])
AC_LANG_CPLUSPLUS
AC_CHECK_HEADER(cstdint)
AX_CXX_COMPILE_STDCXX_0X

CXXFLAGS='-g -Wall -std=gnu++0x'
#CFLAGS='-Wall -std=gnu++0x'

AC_CONFIG_FILES(Makefile bin/Makefile)
AC_OUTPUT

