LIB = imagine

CC = g++
CFLAGS = -DUNIX -O2 -g -Wall
LDFLAGS = -s
ARFLAGS = -r
INCLUDES = -I $(includedir)/newbase -I/usr/local/include/freetype2

# INCLUDES = -I $(includedir)/newbase -I/usr/freeware/include -I/usr/local/include/freetype2


# Common library compiling template

include ../../makefiles/makefile.lib

# This would take forever to compile with -O2, so we use -O0 instead

CFLAGS0 = -DUNIX -O0 -g -Wall
NFmiColorTools.o: NFmiColorTools.cpp
	$(CC) $(CFLAGS0) $(INCLUDES) -c -o obj/$@ $<
