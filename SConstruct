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

#
# Base settings
#
env= DefaultEnvironment()

LINUX=  env["PLATFORM"]=="posix"
OSX=    env["PLATFORM"]=="darwin"
WINDOWS= env["PLATFORM"]=="windows"

env.Append( CPPPATH= [ "./include" ] )

if not WINDOWS: 
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
env_debug= env.Clone() 

if not WINDOWS: 
    debug_flags= [
	    "-Werror",

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
    ]
else:
    debug_flags= []

env_debug.Append( CXXFLAGS=[ "-O0", "-g" ] + debug_flags )


#
# Release settings
#
env_release= env.Clone() 

if not WINDOWS: 
    release_flags= [
        # RELEASEFLAGS from orig. Makefile (for 'release' and 'profile' targets)
        #
        "-Wuninitialized",
    ]
else:
    release_flags= []

env_release.Append( CXXFLAGS="-O2", CPPDEFINES="NDEBUG" )

env_release.Append( CXXFLAGS= release_flags )


#
# Profile settings
#
env_profile= env_release.Clone() 

if not WINDOWS: 
    env_profile.Append( CXXFLAGS="-g -pg" )


#---
# Exceptions to the regular compilation rules (from orig. Makefile)
#
objs= []
shared_objs= []

if DEBUG:
    e= env_debug
    e_O0= env_debug       # No change, anyways
    e_noerror= env_debug.Clone()
    e_noerror["CXXFLAGS"].remove( "-Werror" )
    e_noerror["CXXFLAGS"].append( "-Wno-error" )
else:
    if PROFILE:
        e= env_profile
    else:
        e= env_release
    e_O0= e.Clone()
    e_O0["CXXFLAGS"].remove("-O2")
    e_O0["CXXFLAGS"].append("-O0")
    e_O0["CXXFLAGS"].remove("-Wuninitialized")    # not supported without '-O'
    e_noerror= e    # anyways no -Werror

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
        objs += e.Object( obj_s, fn )
        shared_objs += e.SharedObject( obj_s, fn ) 

e.Library( "smartmet_imagine", objs )
e.SharedLibrary( "smartmet_imagine", shared_objs )


