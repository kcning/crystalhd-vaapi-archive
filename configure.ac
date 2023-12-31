# crystalhd_video package version number
m4_define([crystalhd_video_major_version], [0])
m4_define([crystalhd_video_minor_version], [6])
m4_define([crystalhd_video_micro_version], [9])
m4_define([crystalhd_video_version],
          [crystalhd_video_major_version.crystalhd_video_minor_version.crystalhd_video_micro_version])

# libva package version number
m4_define([libva_sds_version_0_29], [8])
m4_define([libva_sds_package_version_0_29], [0.29-2+sds11])
m4_define([libva_sds_version_0_30], [1])
m4_define([libva_sds_package_version_0_30], [0.30-1+sds1])
m4_define([libva_glx_sds_version_0_30], [5])
m4_define([libva_glx_sds_package_version_0_30], [0.30.4-1+sds6])
m4_define([libva_glx_sds_version_0_31], [1])
m4_define([libva_glx_sds_package_version_0_31], [0.31.0-1+sds1])
m4_define([libva_sds_version], [libva_glx_sds_version_0_31])
m4_define([libva_sds_package_version], [libva_glx_sds_package_version_0_31])

AC_PREREQ([2.57])
AC_INIT([crystalhd_video], [crystalhd_video_version], [gbeauchesne@splitted-desktop.com], [crystalhd-video])
AC_CONFIG_SRCDIR([Makefile.am])
AM_INIT_AUTOMAKE

AM_CONFIG_HEADER([src/config.h])

LIBVA_SDS_VERSION_0_29=libva_sds_version_0_29
LIBVA_SDS_VERSION_0_30=libva_sds_version_0_30
LIBVA_GLX_SDS_VERSION_0_30=libva_glx_sds_version_0_30
LIBVA_GLX_SDS_VERSION_0_31=libva_glx_sds_version_0_31
LIBVA_SDS_VERSION=libva_sds_version
LIBVA_SDS_PACKAGE_VERSION=libva_sds_package_version
AC_SUBST(LIBVA_SDS_PACKAGE_VERSION)

CRYSTALHD_VIDEO_MAJOR_VERSION=crystalhd_video_major_version
CRYSTALHD_VIDEO_MINOR_VERSION=crystalhd_video_minor_version
CRYSTALHD_VIDEO_MICRO_VERSION=crystalhd_video_micro_version
AC_DEFINE([CRYSTALHD_VIDEO_MAJOR_VERSION], [crystalhd_video_major_version], [Major version of the driver])
AC_DEFINE([CRYSTALHD_VIDEO_MINOR_VERSION], [crystalhd_video_minor_version], [Minor version of the driver])
AC_DEFINE([CRYSTALHD_VIDEO_MICRO_VERSION], [crystalhd_video_micro_version], [Micro version of the driver])

CRYSTALHD_VIDEO_LT_LDFLAGS="-avoid-version"
AC_SUBST(CRYSTALHD_VIDEO_LT_LDFLAGS)

AC_DEFUN([AC_DEFINE_01], [
    if test "$2" = "yes" -o "$2" = "guessing yes"; then
        AC_DEFINE($1, 1, $3)
    else
        AC_DEFINE($1, 0, $3)
    fi
])

AC_ARG_ENABLE(glx,
    AC_HELP_STRING([--enable-glx],
                   [enable OpenGL/X11 @<:@default=yes@:>@]),
    [], [enable_glx="yes"])

AC_ARG_ENABLE(debug,
    AC_HELP_STRING([--enable-debug],
                   [enable debugging information]),
    [], [enable_debug="yes"])

AC_DEFINE_01(USE_DEBUG, "$enable_debug",
    [Enable debugging information])

AC_ARG_ENABLE(tracer,
    AC_HELP_STRING([--enable-tracer],
                   [enable CRYSTALHD tracer]),
    [], [enable_tracer="yes"])

AC_DEFINE_01(USE_TRACER, "$enable_tracer",
    [Enable CRYSTALHD tracer])

AC_DISABLE_STATIC
AC_PROG_LIBTOOL
AC_PROG_CC

AC_C_BIGENDIAN
AC_HEADER_STDC
AC_SYS_LARGEFILE

dnl We want pthreads. Try libpthread first, then libc_r (FreeBSD), then PTL.
HAVE_PTHREADS=yes
AC_CHECK_LIB(pthread, pthread_create, , [
    AC_CHECK_LIB(c_r, pthread_create, , [
        AC_CHECK_LIB(PTL, pthread_create, , [
            HAVE_PTHREADS=no
        ])
    ])
])
if test "x$HAVE_PTHREADS" = "xyes"; then
    AC_DEFINE(HAVE_PTHREADS, 1, [Defined if pthreads are available])
fi

dnl Checks for libraries.
AC_CHECK_LIB(rt, timer_create)

dnl Checks for library functions.
AC_CHECK_FUNCS(clock_gettime)

dnl Check for __attribute__((visibility()))
AC_CACHE_CHECK([whether __attribute__((visibility())) is supported],
  crystalhd_cv_visibility_attribute,
  [cat > conftest.c <<EOF
int foo __attribute__ ((visibility ("hidden"))) = 1;
int bar __attribute__ ((visibility ("protected"))) = 1;
EOF
  crystalhd_cv_visibility_attribute=no
  if ${CC-cc} -Werror -S conftest.c -o conftest.s >/dev/null 2>&1; then
    if grep '\.hidden.*foo' conftest.s >/dev/null; then
      if grep '\.protected.*bar' conftest.s >/dev/null; then
	crystalhd_cv_visibility_attribute=yes
      fi
    fi
  fi
  rm -f conftest.[cs]
])
if test $crystalhd_cv_visibility_attribute = yes; then
  AC_DEFINE([HAVE_VISIBILITY_ATTRIBUTE], 1, [Define to use the __attribute__((visibility())) extension])
fi

dnl Check for OpenGL
USE_GLX=1
if test "$enable_glx" != "yes"; then
    USE_GLX=0
fi
GL_DEPS_CFLAGS=""
GL_DEPS_LIBS=""
AC_CHECK_HEADERS([GL/gl.h GL/glext.h GL/glx.h], [], [USE_GLX=0], [
#ifdef HAVE_GL_GL_H
# include <GL/gl.h>
#endif
])
AC_CHECK_LIB(GL, glXCreateContext, [GL_DEPS_LIBS="-lGL -ldl"], [USE_GLX=0])
AC_SUBST(GL_DEPS_CFLAGS)
AC_SUBST(GL_DEPS_LIBS)

dnl Check for VA API
PKG_CHECK_MODULES(LIBVA_DEPS,	  [libva])
PKG_CHECK_MODULES(LIBVA_X11_DEPS, [libva-x11])
PKG_CHECK_MODULES(LIBVA_GLX_DEPS, [libva-glx], [], [USE_GLX=0])

dnl Check for SDS extensions to VA API
AC_CACHE_CHECK([for VA API],
  ac_cv_libva_sds_extensions, [
  saved_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS $LIBVA_DEPS_CFLAGS"
  AC_TRY_COMPILE([
    #include <va/va_version.h>
    #if VA_MAJOR_VERSION == 0 && VA_MINOR_VERSION == 29
    # if !defined(VA_SDS_VERSION) || (VA_SDS_VERSION < $LIBVA_SDS_VERSION_0_29)
    #  error "VA API version >= 0.29.0-sds$LIBVA_SDS_VERSION_0_29 is required"
    # endif
    #elif VA_MAJOR_VERSION == 0 && VA_MINOR_VERSION == 30
    # if !defined(VA_SDS_VERSION) || (VA_SDS_VERSION < $LIBVA_SDS_VERSION_0_30)
    #  error "VA API version >= 0.30.0-sds$LIBVA_SDS_VERSION_0_30 is required"
    # endif
    #elif !VA_CHECK_VERSION(0,31,0)
    # error "VA API version >= 0.31 is required"
    #endif
  ], [],
  [ac_cv_libva_sds_extensions="yes"],
  [ac_cv_libva_sds_extensions="no"])
  CFLAGS="$saved_CFLAGS"
])
VA_VERSION=`$PKG_CONFIG --modversion libva`
VA_MAJOR_VERSION=`echo "$VA_VERSION" | cut -d'.' -f1`
VA_MINOR_VERSION=`echo "$VA_VERSION" | cut -d'.' -f2`
VA_MICRO_VERSION=`echo "$VA_VERSION" | cut -d'.' -f3`
VA_SDS_VERSION=`$PKG_CONFIG libva --variable sdsversion`

VA_VERSION_STR="$VA_VERSION"
if test -n "$VA_SDS_VERSION"; then
    VA_VERSION_STR="$VA_VERSION_STR-sds$VA_SDS_VERSION"
fi

va_full_version_int=`expr ${VA_MAJOR_VERSION:-0} "*" 1000000 + \
                          ${VA_MINOR_VERSION:-0} "*" 10000   + \
                          ${VA_MICRO_VERSION:-0} "*" 100     + \
                          ${VA_SDS_VERSION:-0}`
VA_DRIVER_INIT_FUNC="__vaDriverInit_${VA_MAJOR_VERSION}_${VA_MINOR_VERSION}"
if test $va_full_version_int -ge 00310005; then
    VA_DRIVER_INIT_FUNC="${VA_DRIVER_INIT_FUNC}_${VA_MICRO_VERSION}_sds${VA_SDS_VERSION}"
else
    VA_DRIVER_INIT_FUNC="${VA_DRIVER_INIT_FUNC}_sds"
fi
if test "$ac_cv_libva_sds_extensions" = "yes"; then
    AC_DEFINE_UNQUOTED([VA_DRIVER_INIT_FUNC], [$VA_DRIVER_INIT_FUNC], [Define driver entry-point])
else
    AC_MSG_ERROR([Your VA API SDK does not include SDS extensions])
fi

dnl Check for OpenGL rendering extensions to VA API
AC_CACHE_CHECK([for VA API (GLX extensions)],
  ac_cv_libva_glx_extensions, [
  saved_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS $LIBVA_DEPS_CFLAGS"
  AC_TRY_COMPILE([
    #include <va/va_version.h>
    #if VA_MAJOR_VERSION == 0 && VA_MINOR_VERSION == 30
    # if !defined(VA_SDS_VERSION) || (VA_SDS_VERSION < $LIBVA_GLX_SDS_VERSION_0_30)
    #  error "VA API version >= 0.30.0-sds$LIBVA_GLX_SDS_VERSION_0_30 is required"
    # endif
    #elif VA_MAJOR_VERSION == 0 && VA_MINOR_VERSION == 31
    # if !defined(VA_SDS_VERSION) || (VA_SDS_VERSION < $LIBVA_GLX_SDS_VERSION_0_31)
    #  error "VA API version >= 0.31.0-sds$LIBVA_GLX_SDS_VERSION_0_31 is required"
    # endif
    #else
    # error "This version of VA API is not supported for OpenGL rendering"
    #endif
  ], [],
  [ac_cv_libva_glx_extensions="yes"],
  [ac_cv_libva_glx_extensions="no" USE_GLX=0])
  CFLAGS="$saved_CFLAGS"
])

dnl Check for VA API drivers path
AC_MSG_CHECKING([for VA drivers path])
LIBVA_DRIVERS_PATH=`$PKG_CONFIG libva --variable driverdir`
if test -z "$LIBVA_DRIVERS_PATH"; then
    LIBVA_DRIVERS_PATH="/usr/lib/xorg/modules/drivers"
fi
AC_MSG_RESULT([$LIBVA_DRIVERS_PATH])
AC_SUBST(LIBVA_DRIVERS_PATH)

dnl Check for CRYSTALHD
AC_CHECK_HEADER(
    [libcrystalhd/libcrystalhd_if.h],
    [ac_cv_have_crystalhd_include="yes"],
    [ac_cv_have_crystalhd_include="no"])
AC_CHECK_LIB(
    [crystalhd],
    [DtsDeviceOpen],
    [ac_cv_have_crystalhd_lib="yes"],
    [ac_cv_have_crystalhd_lib="no"])

ac_cv_have_crystalhd="no"
if test "$ac_cv_have_crystalhd_lib:$ac_cv_have_crystalhd_include" = "yes:yes"; then
    ac_cv_have_crystalhd="yes"
    CFLAGS="$CFLAGS -D__LINUX_USER__"
    LIBS="$LIBS -lcrystalhd"
fi
AC_CACHE_CHECK([for Crystal HD], ac_cv_have_crystalhd)

if test "$ac_cv_have_crystalhd" = "no"; then
    AC_MSG_ERROR([you need CRYSTALHD to build this package])
fi

dnl Check for LIBCRYSTALHD version (poor man's check, no pkgconfig joy)
CRYSTALHD_VERSION=`cat << EOF | $CC -E - | grep ^crystalhd_version | cut -d' ' -f2
#include <libcrystalhd/libcrystalhd_version.h>
crystalhd_version DIL_RC_FILE_VERSION
EOF
`

AC_DEFINE_UNQUOTED(USE_GLX, $USE_GLX, [Defined to 1 if GLX is enabled])
AM_CONDITIONAL(USE_GLX, test $USE_GLX -eq 1)

CRYSTALHD_VIDEO_CFLAGS="$LIBVA_DEPS_CFLAGS"
CRYSTALHD_VIDEO_LIBS="$LIBVA_DEPS_LIBS"
if test $USE_GLX -eq 1; then
    CRYSTALHD_VIDEO_CFLAGS="$CRYSTALHD_VIDEO_CFLAGS $GL_DEPS_CFLAGS"
    CRYSTALHD_VIDEO_LIBS="$CRYSTALHD_VIDEO_LIBS $GL_DEPS_LIBS"
fi
AC_SUBST(CRYSTALHD_VIDEO_CFLAGS)
AC_SUBST(CRYSTALHD_VIDEO_LIBS)

AC_OUTPUT([
	Makefile
	src/Makefile
])

dnl Print summary
echo
echo $PACKAGE configuration summary:
echo
echo VA-API version ................... : $VA_VERSION_STR
echo VA-API drivers path .............. : $LIBVA_DRIVERS_PATH
echo CRYSTALHD version ................ : $CRYSTALHD_VERSION
echo GLX support ...................... : $(test $USE_GLX  -eq 1 && echo yes || echo no)
echo Debug? ........................... : $enable_debug
echo Tracer? .......................... : $enable_tracer
echo
