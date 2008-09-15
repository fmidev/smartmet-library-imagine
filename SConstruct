#
# SConstruct for building Imagine
#
# Usage:
#       scons [-j 4] [-Q] [debug=1|profile=1] [objdir=<path>]
#
# Notes:
#       The three variants share the same output and object file names;
#       changing from one version to another will cause a full recompile.
#       This is intentional (instead of keeping, say, out/release/...)
#       for compatibility with existing test suite etc. and because normally
#       different machines are used only for debug/release/profile.
#           -- AKa 11-Sep-2008
#
# Change log:
#       AKa 15-Sep-2008: Simplified (using just one build environment)
#       AKa 11-Sep-2008: Initial version (same as existing Makefile)
#

import os.path

Help(""" 
    Usage: scons [-j 4] [-Q] [debug=1|profile=1] [objdir=<path>] [prefix=<path>]
    
    Or just use 'make release|debug|profile', which point right back to us.
""") 

DEBUG=      int( ARGUMENTS.get("debug", 0) ) != 0
PROFILE=    int( ARGUMENTS.get("profile", 0) ) != 0
RELEASE=    (not DEBUG) and (not PROFILE)     # default

OBJDIR=     ARGUMENTS.get("objdir","obj")
PREFIX=     ARGUMENTS.get("prefix","/usr/")

env= Environment()

LINUX=  env["PLATFORM"]=="posix"
OSX=    env["PLATFORM"]=="darwin"
WINDOWS= env["PLATFORM"]=="windows"

env.Append( CPPPATH= [ "./include" ] )

if WINDOWS: 
    { }     # TBD
else:
    env.Append( CPPDEFINES= ["UNIX"] )
    env.Append( CXXFLAGS= [
        # MAINFLAGS from orig. Makefile ('-fPIC' is automatically added by SCons)
        #
        "-Wall", 
        "-Wno-unused-parameter",
        "-Wno-variadic-macros",
	    
	    # DIFFICULTFLAGS from orig. Makefile (flags that cause some choking,
	    # would be good but not used)
	    #
	    #"-Weffc++",
	    #"-Wredundant-decls",
	    #"-Wshadow",
	    #"-Woverloaded-virtual",
	    #"-Wctor-dtor-privacy",
	    #"-Wold-style-cast",
	    #"-pedantic",
    ] )

if LINUX:
    # Newbase from system install
    #
    env.Append( CPPPATH= [ PREFIX+"/include/smartmet/newbase" ] )

elif OSX:
    # Newbase from local CVS
    #
    env.Append( CPPPATH= [ "../newbase/include" ] )
    env.Append( LIBPATH= [ "../newbase" ] )

    # Boost from Fink
    #
    env.Append( CPPPATH= [ "/sw/include" ] )
    env.Append( LIBPATH= [ "/sw/lib" ] )

env.Append( LIBS= [ "smartmet_newbase" ] )

#
# Freetype2 support
#
if not WINDOWS:
    env.ParseConfig("freetype-config --cflags --libs") 

#
# Cairomm-1.0 support
#
if not WINDOWS:
    env.ParseConfig("pkg-config --cflags --libs cairomm-1.0") 

#
# Other libraries
#
env.Append( LIBS= [ "jpeg", "png", "z" ] )


#
# Debug settings
#
if DEBUG:
    if WINDOWS:
        { } # TBD
    else:
        env.Append( CXXFLAGS=[ "-O0", "-g", "-Werror",
    
            # EXTRAFLAGS from orig. Makefile (for 'debug' target)
            #
            "-ansi",
            "-Wcast-align",
            "-Wcast-qual",
            "-Wconversion",
            "-Winline",
            "-Wno-multichar",
            "-Wno-pmf-conversions",
            "-Woverloaded-virtual",
            "-Wpointer-arith",
            "-Wredundant-decls",
            "-Wsign-promo",
            "-Wwrite-strings",
        ] )

#
# Release settings
#
if RELEASE or PROFILE:
    if WINDOWS:
        { }     # TBD
    else:
        env.Append( CPPDEFINES="NDEBUG",
                    CXXFLAGS= ["-O2",
 
            # RELEASEFLAGS from orig. Makefile (for 'release' and 'profile' targets)
            #
            "-Wuninitialized",
        ] )


#
# Profile settings
#
if PROFILE:
    if WINDOWS:
        { }     # TBD
    else: 
        env.Append( CXXFLAGS="-g -pg" )


#---
# Exceptions to the regular compilation rules (from orig. Makefile)
#
# Note: Samples show 'env.Replace( OBJDIR=... )' to be able to use separate
#       object dir, but this did not work.    -- AKa 15-Sep-2008
#
#env.Replace( OBJDIR=OBJDIR )
#env.Library( "smartmet_imagine", Glob("source/*.cpp") )

objs= []
shared_objs= []

if DEBUG:
    e_O0= env       # No change, anyways
    e_noerror= env.Clone()
    e_noerror["CXXFLAGS"].remove( "-Werror" )
    e_noerror["CXXFLAGS"].append( "-Wno-error" )
else:
    e_O0= env.Clone()
    e_O0["CXXFLAGS"].remove("-O2")
    e_O0["CXXFLAGS"].append("-O0")
    e_O0["CXXFLAGS"].remove("-Wuninitialized")    # not supported without '-O'
    e_noerror= env    # anyways no -Werror

for fn in Glob("source/*.cpp"): 
    s= os.path.basename( str(fn) )
    obj_s= OBJDIR+"/"+ s.replace(".cpp","")
    
    # Using -O0 since -O2 would take so long to compile (really...?)
    #
    if (
        #s == "NFmiColorTools.cpp" or
        False): 
        objs += e_O0.Object( obj_s, fn )
        shared_objs += e_O0.SharedObject( obj_s, fn )
    elif (
         #s == "NFmiEsriShape.cpp" or    # isspace uses an old style cast
         #s == "NFmiImageJpeg.cpp" or    # jpeglib calls cause warnings
         #s == "NFmiPath.cpp" or         # isspace
         #s == "NFmiImage.cpp" or        # sstream causes warnings
        False):
        objs += e_noerror.Object( obj_s, fn )
        shared_objs += e_noerror.SharedObject( obj_s, fn )

    else:
        objs += env.Object( obj_s, fn )
        shared_objs += env.SharedObject( obj_s, fn ) 

env.Library( "smartmet_imagine", objs )
env.SharedLibrary( "smartmet_imagine", shared_objs )


