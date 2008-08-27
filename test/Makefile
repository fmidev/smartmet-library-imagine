PROG = $(patsubst %.cpp,%,$(wildcard *Test.cpp))

MAINFLAGS = -Wall -W -Wno-unused-parameter

EXTRAFLAGS = \
	-Werror \
	-pedantic \
	-Wpointer-arith \
	-Wcast-qual \
	-Wcast-align \
	-Wwrite-strings \
	-Wconversion \
	-Winline \
	-Wctor-dtor-privacy \
	-Wnon-virtual-dtor \
	-Wno-pmf-conversions \
	-Wsign-promo \
	-Wchar-subscripts \
	-Wold-style-cast \
	-Wshadow \
	-Woverloaded-virtual

DIFFICULTFLAGS = \
	-Weffc++ \
	-Wunreachable-code \
	-Wredundant-decls

CAIROMODE = -DIMAGINE_WITH_CAIRO 

# Platform independent Freetype2 support

FT2_LIBS= $(shell freetype-config --libs)
FT2_CFLAGS= $(shell freetype-config --cflags)

# Platform independent Cairomm support

ifneq "$(CAIROMODE)" ""
  CAIRO_LIBS = $(shell pkg-config --libs cairomm-1.0)
  CAIRO_CFLAGS = $(CAIROMODE) $(shell pkg-config --cflags cairomm-1.0)
else
  CAIRO_LIBS =
  CAIRO_CFLAGS = 
endif

CC = g++
CFLAGS = -DUNIX -O0 -g $(MAINFLAGS)

INCLUDES = \
	-I ../include \
	-I /usr/include/ \
	-I /usr/include/smartmet/newbase \
	-I /usr/include/smartmet/regression \
	$(FT2_CFLAGS) \
	$(CAIRO_CFLAGS)

LIBS =  -L .. -lsmartmet_imagine \
	-lsmartmet_newbase \
	-Wl,-rpath,/usr/local/lib \
        -L /usr/local/lib \
	-lboost_regex-gcc41-mt \
	-lboost_filesystem-gcc41-mt \
        -lboost_iostreams-gcc41-mt \
	$(FT2_LIBS) \
	$(CAIRO_LIBS) \
	-lpng -ljpeg -lz

all: $(PROG)
clean:
	rm -f $(PROG) *~

test: $(PROG)
	@echo Running tests:
	@for prog in $(PROG); do \
	./$$prog; \
	done

$(PROG) : % : %.cpp ../libsmartmet_imagine.a
	$(CC) $(CFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBS)

