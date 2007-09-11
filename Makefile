LIB = imagine

MAINFLAGS = -Wall -W -Wno-unused-parameter

EXTRAFLAGS = \
        -ansi \
        -pedantic \
        -Wcast-align \
        -Wcast-qual \
        -Wconversion \
        -Wctor-dtor-privacy \
        -Winline \
        -Wno-multichar \
        -Wno-pmf-conversions \
        -Wold-style-cast \
        -Woverloaded-virtual  \
        -Wpointer-arith \
        -Wredundant-decls \
        -Wshadow \
        -Wsign-promo \
        -Wwrite-strings

RELEASEFLAGS = -Wuninitialized

DIFFICULTFLAGS = -Weffc++ -Wredundant-decls -Wshadow -Woverloaded-virtual -Wunreachable-code

CC = g++
CFLAGS = -DUNIX -O0 -g $(MAINFLAGS) $(EXTRAFLAGS) -Werror
CFLAGS_RELEASE =  -DUNIX -O2 -DNDEBUG $(MAINFLAGS) $(RELEASEFLAGS)
LDFLAGS = -s -static
ARFLAGS = -r
INCLUDES = -I $(includedir) -I $(includedir)/smartmet/newbase -I $(includedir)/freetype2
LIBS = -L $(libdir) -lsmartmet_newbase -lfreetype -ljpeg -lpng -lz


# Common library compiling template

# Installation directories

prosessor := $(shell uname -p)

ifeq ($(origin PREFIX), undefined)
  PREFIX = /usr
else
  PREFIX = $(PREFIX)
endif

ifeq ($(prosessor), x86_64)
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

# What to install

LIBFILE = libsmartmet_$(LIB).a

# How to install

INSTALL_PROG = install -m 775
INSTALL_DATA = install -m 664

# CFLAGS

ifneq (,$(findstring release,$(MAKECMDGOALS)))
  CFLAGS = $(CFLAGS_RELEASE)
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

INCLUDES := -I include $(INCLUDES)

.PHONY: test rpm

# The rules

all: objdir $(LIBFILE)
debug: all
release: all

$(LIBFILE): $(OBJS)
	$(AR) $(ARFLAGS) $(LIBFILE) $(OBJFILES)

clean:
	rm -f $(LIBFILE) $(OBJFILES) *~ source/*~ include/*~

install:
	@mkdir -p $(includedir)/$(LIB)
	@list='$(HDRS)'; \
	for hdr in $$list; do \
	  if [ include/$$hdr -nt $(includedir)/$(LIB)/$$hdr ]; \
	  then \
	    echo $(INSTALL_DATA) include/$$hdr $(includedir)/$(LIB)/$$hdr; \
	  fi; \
	  $(INSTALL_DATA) include/$$hdr $(includedir)/$(LIB)/$$hdr; \
	done
	@mkdir -p $(libdir)
	$(INSTALL_DATA) $(LIBFILE) $(libdir)/$(LIBFILE)

depend:
	makedepend $(INCLUDES)

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

# -include Dependencies
# DO NOT DELETE THIS LINE -- make depend depends on it.
