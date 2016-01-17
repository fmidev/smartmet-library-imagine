LIB = imagine

# Installation directories

processor := $(shell uname -p)

ifeq ($(origin PREFIX), undefined)
  PREFIX = /usr
else
  PREFIX = $(PREFIX)
endif

ifeq ($(processor), x86_64)
  libdir = $(PREFIX)/lib64
else
  libdir = $(PREFIX)/lib
endif

bindir = $(PREFIX)/bin
includedir = $(PREFIX)/include
datadir = $(PREFIX)/share
objdir = obj

# Compiler options

DEFINES = -DUNIX -D_REENTRANT -DFMI_COMPRESSION -DBOOST -DBOOST_IOSTREAMS_NO_LIB

ifeq ($(CXX), clang++)

 FLAGS = \
	-std=c++11 -fPIC -MD \
	-Weverything \
	-Wno-c++98-compat \
	-Wno-float-equal \
	-Wno-padded \
	-Wno-missing-prototypes

 INCLUDES = \
	-isystem $(includedir) \
	-isystem $(includedir)/smartmet \
	-isystem $(includedir)/smartmet/newbase \
	`freetype-config --cflags`

else

 FLAGS = -std=c++0x -fPIC -MD -Wall -W -Wno-unused-parameter

 FLAGS_DEBUG = \
	-Wcast-align \
	-Winline \
	-Wno-multichar \
	-Wno-pmf-conversions \
	-Woverloaded-virtual  \
	-Wpointer-arith \
	-Wcast-qual \
	-Wredundant-decls \
	-Wwrite-strings \
	-Wsign-promo

 FLAGS_RELEASE = -Wuninitialized

 INCLUDES = \
	-I$(includedir) \
	-I$(includedir)/smartmet \
	-I$(includedir)/smartmet/newbase \
	`freetype-config --cflags`

endif

# Compile options in detault, debug and profile modes

CFLAGS         = $(DEFINES) $(FLAGS) $(FLAGS_RELEASE) -DNDEBUG -O2
CFLAGS_DEBUG   = $(DEFINES) $(FLAGS) $(FLAGS_DEBUG)   -Werror  -O0 -g
CFLAGS_PROFILE = $(DEFINES) $(FLAGS) $(FLAGS_PROFILE) -DNDEBUG -O2 -g -pg

LIBS = -L$(libdir) \
	-lboost_filesystem \
	-lboost_regex \
	-lboost_thread \
	-lgdal \
	`freetype-config --libs` \
	-ljpeg -lpng -lz

# rpm variables

rpmsourcedir = /tmp/$(shell whoami)/rpmbuild

rpmerr = "There's no spec file (imagine.spec). RPM wasn't created. Please make a spec file or copy and rename it into imagine.spec"

# What to install

LIBFILE = libsmartmet_$(LIB).so

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

# The files to be compiled

SRCS = $(wildcard source/*.cpp)
HDRS = $(wildcard include/*.h)
OBJS = $(patsubst %.cpp, obj/%.o, $(notdir $(SRCS)))

INCLUDES := -Iinclude $(INCLUDES)

.PHONY: test rpm

# The rules

all: objdir $(LIBFILE)
debug: all
release: all
profile: all

$(LIBFILE): $(OBJS)
	$(CXX) $(CFLAGS) -shared -rdynamic -o $(LIBFILE) $(OBJS) $(LIBS)

clean:
	rm -f $(LIBFILE) *~ source/*~ include/*~
	rm -rf $(objdir)

format:
	clang-format -i -style=file include/*.h source/*.cpp test/*.cpp

install:
	@mkdir -p $(includedir)/smartmet/$(LIB)
	@list='$(HDRS)'; \
	for hdr in $$list; do \
	  HDR=$$(basename $$hdr); \
	  echo $(INSTALL_DATA) $$hdr $(includedir)/smartmet/$(LIB)/$$HDR; \
	  $(INSTALL_DATA) $$hdr $(includedir)/smartmet/$(LIB)/$$HDR; \
	done
	@mkdir -p $(libdir)
	$(INSTALL_PROG) $(LIBFILE) $(libdir)/$(LIBFILE)

test:
	cd test && make test

objdir:
	@mkdir -p $(objdir)

rpm: clean
	if [ -e imagine.spec ]; \
	then \
	  smartspecupdate imagine.spec ; \
	  mkdir -p $(rpmsourcedir) ; \
	  tar -C ../ -cf $(rpmsourcedir)/libsmartmet-$(LIB).tar imagine ; \
	  gzip -f $(rpmsourcedir)/libsmartmet-$(LIB).tar ; \
	  TAR_OPTIONS=--wildcards rpmbuild -v -ta $(rpmsourcedir)/libsmartmet-$(LIB).tar.gz ; \
	  rm -f $(rpmsourcedir)/libsmartmet-$(LIB).tar.gz ; \
	else \
	  echo $(rpmerr); \
	fi;

.SUFFIXES: $(SUFFIXES) .cpp

obj/%.o: %.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c -o $@ $<

ifneq ($(wildcard obj/*.d),)
-include $(wildcard obj/*.d)
endif
