dnl Process this file with autoconf to produce a configure script.
AC_INIT(unix/main.c)
AC_CONFIG_FILES([config.mak])

dnl Checks for programs.
AC_PROG_CC

dnl Checks for endianness
AC_C_BIGENDIAN

dnl Checks for libraries.
dnl Replace `main' with a function in -lX11:
AC_CHECK_LIB(X11, XInitThreads, AC_DEFINE(HAVE_XINITTHREADS))
dnl Replace `main' with a function in -lXext:
dnl AC_CHECK_LIB(Xext, main)
dnl Replace `main' with a function in -libs:
dnl AC_CHECK_LIB(ibs, main)
dnl AM_PATH_GTK(1.2.0,AC_DEFINE(HAVE_GTK),AC_MSG_RESULT(GTK GUI will not be built!))

AC_ARG_ENABLE(gtk,
  AC_HELP_STRING(
    [--disable-gtk],
    [disable the gtk library, [default=enabled]]
  ),
  [enable_gtk=$enableval],
  [enable_gtk=yes]
)

if test "$enable_gtk" = yes; then
  AM_PATH_GTK_2_0(2.2.0,AC_DEFINE(HAVE_GTK),AC_MSG_RESULT(GTK GUI will not be built!))
fi
AM_PATH_SDL(1.2.0,AC_DEFINE(HAVE_SDL),AC_MSG_RESULT(SDL will be disables. Using plain X11 code instead!))
AM_PATH_ALSA(1.0.0,AC_DEFINE(HAVE_ALSA),AC_MSG_RESULT(ALSA will be disables. Using SDL sound instead!))
AC_CHECK_HEADER(sys/soundcard.h,AC_DEFINE(HAVE_SYS_SOUNDCARD_H),AC_MSG_RESULT(OSS sound support will not be built!))

dnl Checks for header files.
AC_PATH_XTRA
dnl AC_SUBST(X_CFLAGS)
dnl AC_SUBST(X_LIBS)
dnl AC_HEADER_STDC
dnl AC_CHECK_HEADERS(fcntl.h malloc.h sys/ioctl.h sys/time.h unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
dnl AC_C_CONST
dnl AC_TYPE_SIZE_T
dnl AC_HEADER_TIME

dnl Checks for library functions.
dnl AC_PROG_GCC_TRADITIONAL
dnl AC_FUNC_MEMCMP
dnl AC_TYPE_SIGNAL
dnl AC_CHECK_FUNCS(getcwd gettimeofday select)

AC_OUTPUT()
