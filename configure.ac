AC_PREREQ(2.61)
AC_INIT([pschat], [0.0.2])
AC_PROG_CXX

AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall foreign])

AC_CONFIG_HEADERS([config.h])

AC_CHECK_HEADERS([signal.h fcntl.h locale.h stdint.h stdlib.h unistd.h math.h])
AC_CHECK_FUNCS(gettimeofday chdir chmod getopt fmod)

#AC_CHECK_LIB(m, fmod)
AC_CHECK_LIB(pthread, pthread_create)
AC_CHECK_LIB(archive, archive_write_open_filename)
AC_CHECK_LIB(confuse, cfg_parse)
AC_CHECK_LIB(sqlite3, sqlite3_exec)

AC_CHECK_LIB([gtk-x11-2.0], [gtk_main], [], [AC_MSG_ERROR([gtk+-2.x is missing or not built with X support])])
AC_CHECK_LIB(glib-2.0, g_print)
AC_CHECK_LIB(ssl, EVP_EncryptInit)
#AC_CHECK_LIB(crypto, )

# GLIB
PKG_CHECK_MODULES(GLIB, glib-2.0 gobject-2.0)
AC_SUBST(GLIB_LIBS)
AC_SUBST(GLIB_CFLAGS)

# GTK+2
PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.8)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

#profile
AC_ARG_ENABLE(
profile,
[ --enable-profile=ARG enable support for profiling (default=no) ],
[enable_profile=$enableval],
[enable_profile=no]
)
if test "$enable_profile" = "yes"; then
CXXFLAGS="$CFLAGS -pg"
AC_DEFINE(ENABLE_PROFILE,1,[profile option])
else
AC_DEFINE(ENABLE_PROFILE,1,[profile option])
fi

#debug
AC_ARG_ENABLE(
debug,
[--enable-debug=ARG enable support for debugging (default=no)],
[enable_debug=$enableval],
[enable_debug=no]
)
if test "$enable_debug" = "yes"; then
CFLAGS="-O0 -g -Wall -Werror -pedantic"
AC_DEFINE(ENABLE_DEBUG,1,[debug option])
else
AC_DEFINE(ENABLE_DEBUG,0,[debug option])
fi

#release
AC_ARG_ENABLE(
release,
[--enable-release=ARG enable release version (default=no)],
[enable_release=$enableval],
[enable_release=no]
)
if test "$enable_release" = "yes"; then
CXXFLAGS="-O3 -mtune=native -march=native -pipe"
AC_DEFINE(ENABLE_RELEASE,1,[release option])
else
AC_DEFINE(ENABLE_RELEASE,0,[release option])
fi

CXXFLAGS="$CXXFLAGS"

DISTCLEANFILES="Makefile.in"
AC_SUBST(DISTCLEANFILES)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
