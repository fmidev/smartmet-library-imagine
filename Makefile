LIB = imagine

MAINFLAGS = -Wall -W -Wno-unused-parameter

EXTRAFLAGS = -Werror -pedantic -Wpointer-arith -Wcast-qual \
	-Wcast-align -Wwrite-strings -Wconversion -Winline \
	-Wctor-dtor-privacy -Wnon-virtual-dtor -Wno-pmf-conversions \
	-Wsign-promo -Wchar-subscripts -Wold-style-cast

DIFFICULTFLAGS = -Weffc++ -Wredundant-decls -Wshadow -Woverloaded-virtual -Wunreachable-code

CC = g++
CFLAGS = -DUNIX -O0 -g $(MAINFLAGS) $(EXTRAFLAGS) -Werror
CFLAGS_RELEASE =  -DUNIX -O2 -DNDEBUG $(MAINFLAGS)
LDFLAGS = -s
ARFLAGS = -r
INCLUDES = -I $(includedir) -I $(includedir)/newbase -I /usr/include/freetype2
LIBS = -L ../../../../lib -lnewbase -lfreetype -ljpeg -lpng -lz

# INCLUDES = -I $(includedir)/newbase -I/usr/freeware/include -I/usr/local/include/freetype2


# Common library compiling template

include ../../makefiles/makefile.lib

# This would take forever to compile with -O2, so we use -O0 instead

CFLAGS0 = -DUNIX -O0 -g -Wall
NFmiColorTools.o: NFmiColorTools.cpp
	$(CC) $(CFLAGS0) $(INCLUDES) -c -o $(objdir)/$@ $<

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
