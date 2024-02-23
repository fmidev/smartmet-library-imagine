PROG = $(patsubst %.cpp,%,$(wildcard *Test.cpp))

CAIROMODE = # -DIMAGINE_WITH_CAIRO

REQUIRES :=
ifneq "$(CAIROMODE)" ""
REQUIRES += cairo
endif

include $(shell echo $${PREFIX-/usr})/share/smartmet/devel/makefile.inc

DEFINES = -DUNIX -D_REENTRANT -DFMI_COMPRESSION -DBOOST -DBOOST_IOSTREAMS_NO_LIB

# Platform independent Freetype2 support

FT2_LIBS= $(shell freetype-config --libs)
FT2_CFLAGS= $(shell freetype-config --cflags)

INCLUDES += \
	-I../imagine \
	-I/usr/include/smartmet/ \
	-I/usr/include/smartmet/newbase \
	-I/usr/include/smartmet/regression \
	$(FT2_CFLAGS)

LIBS +=  ../libsmartmet-imagine.so \
	-lsmartmet-newbase \
	-lsmartmet-macgyver \
	-lboost_regex \
	-lboost_filesystem \
        -lboost_iostreams \
        -lboost_thread \
        -lboost_system \
	$(FT2_LIBS) \
	$(REQUIRES_LIBS) \
	-lz -lpthread -lrt

all: $(PROG)
clean:
	rm -f $(PROG) *~


test: $(PROG)
	@echo Running tests:
	@for prog in $(PROG); do \
	./$$prog; \
	done

$(PROG) : % : %.cpp ../libsmartmet-imagine.so
	$(CXX) $(CFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBS)

