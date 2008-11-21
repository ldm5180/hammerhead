## Process this file with automake to produce Makefile.in

#to do test coverage analysis using gcov, uncomment
#CFLAGS = -g -O0 -Wall -W -fprofile-arcs -ftest-coverage
#CXXFLAGS=-g -O0 -Wall -W -Wshadow -Wunused-variable -Wunused-parameter -Wunused-function -Wunused -Wno-system-headers -Wno-deprecated -Woverloaded-virtual -Wwrite-strings -fprofile-arcs -ftest-coverage
#LDFLAGS=-fprofile-arcs -ftest-coverage -lgcov

include $(top_srcdir)/common.am

SUBDIRS = cal util asn.1 client hab data-manager



#
# Set The SDL Version.
#
# For detailed explanation of this versioning scheme, 
# See: http://sourceware.org/autobook/autobook/autobook_91.html#SEC91
#

# The number of the current interface exported by the library. A current 
# value of `0', means that you are calling the interface exported by this 
# library interface 0. 
#
# Increment whenever the interface changes
INTERFACE_VER = 2
export INTERFACE_VER

# The implementation number of the most recent interface exported by this 
# library. In this case, a revision value of `0' means that this is the first
# implementation of the interface.
#
# If the next release of this library exports the same interface, but has a 
# different implementation (perhaps some bugs have been fixed), the revision 
# number will be higher, but current number will be the same. In that case, 
# when given a choice, the library with the highest revision will always be 
# used by the runtime loader. 
#
# Increment this with every buf-fix release;
# Set this to 0 when INTERFACE_VER changes
INTERFACE_REV = 0
export INTERFACE_REV

# The number of previous additional interfaces supported by this library. If 
# age were `2', then this library can be linked into executables which were 
# built with a release of this library that exported the current interface 
# number, current, or any of the previous two interfaces.
#
# By definition age must be less than or equal to current. At the outset, 
# only the first ever interface is implemented, so age can only be `0'. 
#
# Review this whenever INTERFACE_VER changes.
INTERFACE_AGE = 0
export INTERFACE_AGE

