PROG = $(patsubst %.cpp,%,$(wildcard *.cpp))

MAINFLAGS = -Wall -W -Wno-unused-parameter

EXTRAFLAGS = -Werror -pedantic -Wpointer-arith -Wcast-qual \
	-Wcast-align -Wwrite-strings -Wconversion -Winline \
	-Wctor-dtor-privacy -Wnon-virtual-dtor -Wno-pmf-conversions \
	-Wsign-promo -Wchar-subscripts -Wold-style-cast \
	-Wshadow -Woverloaded-virtual

DIFFICULTFLAGS = -Weffc++ -Wunreachable-code -Wredundant-decls

CC = g++
CFLAGS = -DUNIX -O0 -g $(MAINFLAGS)
INCLUDES = -I ../include -I ../../../../../include/ -I ../../../../../include/newbase -I ../../../../../include/regression -I /usr/include/freetype2
LIBS =  -L .. -limagine -L ../../../../../lib -lnewbase -lfreetype -lpng -ljpeg -lz

all: $(PROG)
clean:
	rm -f $(PROG) *~

$(PROG) : % : %.cpp ../libimagine.a
	$(CC) $(CFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBS)
