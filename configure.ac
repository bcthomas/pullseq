#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([pullseq], [1.1.0], [bct.x42@gmail.com])
#AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/pullseq.c])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([z],[gzopen])
AC_CHECK_LIB([pcre2-8],[pcre2_config_8], [], [
              echo "  Error! You need to have libpcre2 installed."
              echo "  If you have PCRE2 installed in a non-standard"
              echo "  location (e.g. as with brew on a mac), you'll"
              echo "  need to update the CFLAGS/LDFLAGS env"
              echo "  variables. See the README for more details."
              exit -1
              ])

# Checks for header files.
AC_CHECK_HEADERS([pcre2.h inttypes.h limits.h stddef.h stdlib.h string.h])

# this no longer works, but the above does... will debug later
#AC_CHECK_HEADERS([pcre2.h], [], [
#                  echo "  Error! You need to have libpcre2 headers installed."
#                  echo "  If you have PCRE2 installed in a non-standard"
#                  echo "  location (e.g. as with brew on a mac), you'll"
#                  echo "  need to update the CFLAGS/LDFLAGS env"
#                  echo "  variables. See the README for more details."
#                  exit -1
#                 ])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([strlen memset strchr strerror strtol])


AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT
