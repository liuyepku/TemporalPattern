
# utility/src/Makefile.am

CXXFLAGS = $(DEBUG_FLAGS)
INCLUDES = $(STL_PATH)

lib_LTLIBRARIES = libutility.la
check_PROGRAMS = utiltest
TESTS = $(check_PROGRAMS)
LDADD = libutility.la

include_HEADERS = \
 defbool.hpp \
 define.hpp \
 Chars.hpp \
 stlstream.hpp \
 stlutil.hpp \
 strconv.hpp \
 ttime.hpp \
 ustream.hpp \
 bool3.hpp

libutility_la_SOURCES = \
 defbool.cpp \
 stlstream.cpp \
 stlutil.cpp \
 strconv.cpp \
 ttime.cpp \
 ustream.cpp

utiltest_SOURCES = utiltest.cpp
