#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([myftp], [1.0], [bug@infotm.com])
AM_INIT_AUTOMAKE(myftp,1.0)
AC_CONFIG_SRCDIR([myftp.c])
AC_CONFIG_HEADERS([config.h])
# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h netdb.h netinet/in.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([getcwd gethostbyname memset mkdir socket])

AC_OUTPUT([Makefile])
