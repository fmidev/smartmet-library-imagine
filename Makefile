LIB = imagine

MAINFLAGS = -Wall -W -Wno-unused-parameter

EXTRAFLAGS = -Werror -pedantic -Wpointer-arith -Wcast-qual \
	-Wcast-align -Wwrite-strings -Wconversion -Winline \
	-Wctor-dtor-privacy -Wnon-virtual-dtor -Wno-pmf-conversions \
	-Wsign-promo -Wchar-subscripts -Wold-style-cast -Wunreachable-code

DIFFICULTFLAGS = -Weffc++ -Wredundant-decls -Wshadow -Woverloaded-virtual

CC = g++
CFLAGS = -DUNIX -g $(MAINFLAGS) $(EXTRAFLAGS) -Werror
CFLAGS_RELEASE =  -DUNIX -O2 -DNDEBUG $(MAINFLAGS)
LDFLAGS = -s
ARFLAGS = -r
INCLUDES = -I $(includedir)/newbase -I /usr/local/include/freetype2

# INCLUDES = -I $(includedir)/newbase -I/usr/freeware/include -I/usr/local/include/freetype2


# Common library compiling template

include ../../makefiles/makefile.lib

# This would take forever to compile with -O2, so we use -O0 instead

CFLAGS0 = -DUNIX -O0 -g -Wall
NFmiColorTools.o: NFmiColorTools.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c -o obj/$@ $<

# isspace uses an old style cast, cannot help it
NFmiEsriShape.o: NFmiEsriShape.cpp
	$(CC) $(CFLAGS) -Wno-error $(INCLUDES) -c -o obj/$@ $<

# jpeglib calls cause warnings, cannot help it
NFmiImageJpeg.o: NFmiImageJpeg.cpp
	$(CC) $(CFLAGS) -Wno-error $(INCLUDES) -c -o obj/$@ $<

# isspace causes warnings, cannot help it
NFmiPath.o: NFmiPath.cpp
	$(CC) $(CFLAGS) -Wno-error $(INCLUDES) -c -o obj/$@ $<