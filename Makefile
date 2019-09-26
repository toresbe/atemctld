CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g -std=gnu++11 -Wall -Wextra -I. $(shell pkg-config --cflags libpistache libatemkontroll)
LDFLAGS=-g 
LDLIBS=$(shell pkg-config --libs libpistache libatemkontroll)

PROG=atemctld
SRCS=atemctld.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all:		$(PROG)

$(PROG):	$(OBJS)
		$(CXX) $(LDFLAGS) -o $(PROG) $(OBJS) $(LDLIBS) 

clean:
	    $(RM) $(OBJS)

distclean: clean
	    $(RM) $(PROG)
