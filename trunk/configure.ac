#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([shamir_OTP], [.1], [chrismiceli@gmail.com])
AM_INIT_AUTOMAKE
LT_PREREQ([2.2])
AC_CONFIG_SRCDIR([src/create_profile.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AM_PROG_CC_C_O

# Configure options: --enable-debug[=no]
AC_ARG_ENABLE([debug],
    AS_HELP_STRING([--enable-debug], [Enable debug code]))

AS_IF([test "x$enable_debug" = "xyes"], [
   AC_DEFINE([SHAMIR_DEBUG], [1], [Debug mode])
  ])

TRY_DOXYGEN

AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([docs/Doxyfile])])

# Checks for libraries.
have_gmp=no
AC_SEARCH_LIBS([__gmpz_init], [gmp], [have_gmp=yes], [have_gmp=no])
if test "x${have_gmp}" = xyes; then
   AC_CHECK_HEADERS([gmp.h])
fi

if test "x${have_gmp}" = xno; then
   AC_MSG_ERROR([
------------------------------------------------
 The Gnu Multiprecision library is required 
 to build the Shamir One-Time Password library.
 Check 'config.log' for more information.
------------------------------------------------])
fi

have_pam=no
AC_SEARCH_LIBS([pam_set_data], [pam], [have_pam=yes], [have_pam=no])
if test "x${have_pam}" = xyes; then
   AC_CONFIG_FILES([pam-module/Makefile])
fi

if test "x${have_pam}" = xno; then
   AC_MSG_WARN([
------------------------------------------------
  The pam development tools could not be found.
  The pam module will not be built.
  Check 'config.log' for more information.
------------------------------------------------])
fi

# Checks for header files.
AC_CHECK_HEADERS([stdio.h string.h stdlib.h errno.h math.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 libshamir_OTP/Makefile
                 src/Makefile
                 docs/Makefile])
AC_OUTPUT

echo "
   Shamir One-Time Password ($PACKAGE_NAME) version $PACKAGE_VERSION
   Prefix........: $prefix
   Debug Build...: $enable_debug
   C Compiler....: $CC $CFLAGS
   Linker........: $LD $LDFLAGS $LIBS
"
