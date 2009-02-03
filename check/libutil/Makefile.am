include $(top_srcdir)/common.am

TESTS = check-libutil

check_PROGRAMS = check-libutil

nodist_check_libutil_SOURCES = \
	../check-common.c \
	check-libutil.c \
	check-libutil-hab-tests.c

check_libutil_CFLAGS = $(CHECK_CFLAGS) -I.. $(CFLAGS_BIONET_UTIL)

check_libutil_LDFLAGS = $(CHECK_LIBS) $(LDFLAGS_BIONET_UTIL)

check_libutil_LDADD = $(LIBS_BIONET_UTIL)
