LIB = imagine

MAINFLAGS = -Wall -W -Wno-unused-parameter -Wno-variadic-macros -fPIC

EXTRAFLAGS = \
        -ansi \
        -Wcast-align \
        -Wcast-qual \
        -Wconversion \
        -Winline \
        -Wno-multichar \
        -Wno-pmf-conversions \
        -Woverloaded-virtual  \
        -Wpointer-arith \
        -Wredundant-decls \
        -Wsign-promo \
        -Wwrite-strings

RELEASEFLAGS = -Wuninitialized

DIFFICULTFLAGS = \
	-Weffc++ \
	-Wredundant-decls \
	-Wshadow \
	-Woverloaded-virtual \
        -Wctor-dtor-privacy \
        -Wold-style-cast \
        -pedantic \
	-Wunreachable-code

CC = g++
ARFLAGS = -rs

ifeq "$(shell uname -s)" "Darwin"
  NEWBASE_PATH = $(HOME)/Work/IL/cvs/newbase
  PREFIX = /sw
else
  ifeq ($(origin PREFIX), undefined)
    PREFIX = /usr
  else
    PREFIX = $(PREFIX)
  endif
endif

# Platform independent Freetype2 support    -- AKa 17-Jul-2008
#
FT2_LIBS= $(shell freetype-config --libs)
FT2_CFLAGS= $(shell freetype-config --cflags)

INCLUDES = -I$(includedir) \
	-I$(includedir)/smartmet/newbase \
	-I/usr/local/include/boost-1_35 \
	$(FT2_CFLAGS)

#INCLUDES = -I$(includedir) \
#	-I$(includedir)/smartmet/newbase \
#        -I/usr/local/include/boost-1_35 \
#	-I/usr/include/freetype2

LIBS = -L$(libdir) \
	-lsmartmet_newbase \
	$(FT2_LIBS) -ljpeg -lpng -lz

# Platform independent Cairomm support      -- AKa 25-Aug-2008
#
LIBS += $(shell pkg-config --libs cairomm-1.0)
MAINFLAGS += $(CAIRO_FLAGS) $(shell pkg-config --cflags cairomm-1.0)

ifneq "$(NEWBASE_PATH)" ""
  INCLUDES += -I$(NEWBASE_PATH)/include
  LIBS += -L$(NEWBASE_PATH)
endif

# Default compile options

CFLAGS =  -DUNIX -O2 -DNDEBUG $(MAINFLAGS) $(RELEASEFLAGS)

# Special mode options

CFLAGS_DEBUG = -DUNIX -O0 -g $(MAINFLAGS) $(EXTRAFLAGS) -Werror
CFLAGS_PROFILE =  -DUNIX -O2 -g -pg -DNDEBUG $(MAINFLAGS) $(RELEASEFLAGS)

# Common library compiling template

# Installation directories

processor := $(shell uname -p)

ifeq ($(processor), x86_64)
  libdir = $(PREFIX)/lib64
else
  libdir = $(PREFIX)/lib
endif

bindir = $(PREFIX)/bin
includedir = $(PREFIX)/include
objdir = obj

# rpm variables

rpmsourcedir = /smartmet/src/redhat/SOURCES
rpmerr = "There's no spec file ($(LIB).spec). RPM wasn't created. Please make a spec file or copy and rename it into $(LIB).spec"

rpmversion := $(shell grep "^Version:" $(LIB).spec  | cut -d\  -f 2 | tr . _)
rpmrelease := $(shell grep "^Release:" $(LIB).spec  | cut -d\  -f 2 | tr . _)

# What to install

LIBFILE = libsmartmet_$(LIB).a

# How to install

INSTALL_PROG = install -m 775
INSTALL_DATA = install -m 664

# Compile option overrides

ifneq (,$(findstring debug,$(MAKECMDGOALS)))
  CFLAGS = $(CFLAGS_DEBUG)
endif

ifneq (,$(findstring profile,$(MAKECMDGOALS)))
  CFLAGS = $(CFLAGS_PROFILE)
endif

# Compilation directories

vpath %.cpp source
vpath %.h include
vpath %.o $(objdir)

# The files to be compiled

SRCS = $(patsubst source/%,%,$(wildcard source/*.cpp))
HDRS = $(patsubst include/%,%,$(wildcard include/*.h))
OBJS = $(SRCS:%.cpp=%.o)

OBJFILES = $(OBJS:%.o=obj/%.o)

INCLUDES := -Iinclude $(INCLUDES)

# For make depend:

ALLSRCS = $(wildcard *.cpp source/*.cpp)

.PHONY: test rpm

# The rules

all: objdir $(LIBFILE)
debug: all
release: all
profile: all

$(LIBFILE): $(OBJS)
	$(AR) $(ARFLAGS) $(LIBFILE) $(OBJFILES)

clean:
	rm -f $(LIBFILE) $(OBJFILES) *~ source/*~ include/*~

install:
	@mkdir -p $(includedir)/smartmet/$(LIB)
	@list='$(HDRS)'; \
	for hdr in $$list; do \
	  echo $(INSTALL_DATA) include/$$hdr $(includedir)/smartmet/$(LIB)/$$hdr; \
	  $(INSTALL_DATA) include/$$hdr $(includedir)/smartmet/$(LIB)/$$hdr; \
	done
	@mkdir -p $(libdir)
	$(INSTALL_DATA) $(LIBFILE) $(libdir)/$(LIBFILE)

depend:
	gccmakedep -fDependencies -- $(CFLAGS) $(INCLUDES) -- $(ALLSRCS)

test:
	cd test && make test

html::
	mkdir -p /data/local/html/lib/$(LIB)
	doxygen $(LIB).dox

objdir:
	@mkdir -p $(objdir)

rpm: clean depend
	if [ -e $(LIB).spec ]; \
	then \
	  tar -C ../ -cf $(rpmsourcedir)/libsmartmet-$(LIB).tar $(LIB) ; \
	  gzip -f $(rpmsourcedir)/libsmartmet-$(LIB).tar ; \
	  TAR_OPTIONS=--wildcards rpmbuild -ta $(rpmsourcedir)/libsmartmet-$(LIB).tar.gz ; \
	else \
	  echo $(rpmerr); \
	fi;

tag:
	cvs -f tag 'libsmartmet_$(LIB)_$(rpmversion)-$(rpmrelease)' .

headertest:
	@echo "Checking self-sufficiency of each header:"
	@echo
	@for hdr in $(HDRS); do \
	echo $$hdr; \
	echo "#include \"$$hdr\"" > /tmp/$(LIB).cpp; \
	echo "int main() { return 0; }" >> /tmp/$(LIB).cpp; \
	$(CC) $(CFLAGS) $(INCLUDES) -o /dev/null /tmp/$(LIB).cpp $(LIBS); \
	done

.SUFFIXES: $(SUFFIXES) .cpp

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $(objdir)/$@ $<

# This would take forever to compile with -O2, so we use -O0 instead

CFLAGS0 = -DUNIX -O0 -g -Wall
# NFmiColorTools.o: NFmiColorTools.cpp
#	$(CC) $(CFLAGS0) $(INCLUDES) -c -o $(objdir)/$@ $<

# isspace uses an old style cast, cannot help it
NFmiEsriShape.o: NFmiEsriShape.cpp
	$(CC) $(CFLAGS) -Wno-error $(INCLUDES) -c -o $(objdir)/$@ $<

# jpeglib calls cause warnings, cannot help it
NFmiImageJpeg.o: NFmiImageJpeg.cpp
	$(CC) $(CFLAGS) -Wno-error $(INCLUDES) -c -o $(objdir)/$@ $<

# isspace causes warnings, cannot help it
NFmiPath.o: NFmiPath.cpp
	$(CC) $(CFLAGS) -Wno-error $(INCLUDES) -c -o $(objdir)/$@ $<

# sstream causes warnings, cannot help it
NFmiImage.o: NFmiImage.cpp
	$(CC) $(CFLAGS) -Wno-error $(INCLUDES) -c -o $(objdir)/$@ $<

-include Dependencies
