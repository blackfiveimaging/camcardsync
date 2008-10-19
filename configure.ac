#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([camcardsync], [0.1.1], [blackfive@fakenhamweb.co.uk])
AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_SRCDIR([camcardsync.cpp])
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([1.9 foreign])

AM_GNU_GETTEXT_VERSION(0.11.5)
AM_GNU_GETTEXT([external])

# Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_CPP

# Checks for libraries.
PKG_CHECK_MODULES([GLIB2], [glib-2.0])
PKG_CHECK_MODULES([GTK2], [gtk+-2.0])
PKG_CHECK_MODULES([GDKPIXBUF2], [gdk-pixbuf-2.0])
# PKG_CHECK_MODULES([HAL], [hal], [CXXFLAGS="$CXXFLAGS -DHAVE_HAL"])
PKG_CHECK_MODULES([HAL], [hal], [AC_DEFINE([HAVE_HAL],[1],[Set if HAL is available])], [DUMMY=NOHAL])

AC_CHECK_LIB(m,pow, LIBM_LIBS="-lm")
AC_SUBST([LIBM_LIBS])

AC_CHECK_LIB([gnugetopt], [getopt_long], [GETOPT_LIBS=-lgnugetopt])
AC_SUBST([GETOPT_LIBS])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_STAT
AC_CHECK_FUNCS([memset mkdir strcasecmp strdup strncasecmp])

AC_FUNC_CLOSEDIR_VOID
AC_HEADER_DIRENT
AC_STRUCT_TM 

AC_CONFIG_FILES([Makefile po/Makefile.in ])
AC_OUTPUT
