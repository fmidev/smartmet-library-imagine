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
# Windows usage:
#	Run 'vcvars32.bat' or similar before using us, to give right
#	env.var. setups for Visual C++ 2008 command line tools & headers.
#
# Change log:
#       AKa 15-Sep-2008: Simplified (using just one build environment)
#       AKa 11-Sep-2008: Initial version (same as existing Makefile)
#

# Show usage log from Imagine?
#
IMAGINE_USAGE= False    #True

import os.path

Help(""" 
    Usage: scons [-j 4] [-Q] [debug=1|profile=1] [objdir=<path>] [prefix=<path>]
                 [windows_boost_path=<path>] [windows_prebuilt_path=<path>]
    
    Or just use 'make release|debug|profile', which point right back to us.
""") 

Decider("MD5-timestamp") 

DEBUG=      int( ARGUMENTS.get("debug", 0) ) != 0
PROFILE=    int( ARGUMENTS.get("profile", 0) ) != 0
RELEASE=    (not DEBUG) and (not PROFILE)     # default

OBJDIR=     ARGUMENTS.get("objdir","obj")
PREFIX=     ARGUMENTS.get("prefix","/usr/")
LIBDIR=     ARGUMENTS.get("libdir","/usr/lib")

# Installed from 'boost_1_36_0_setup.exe' from BoostPro Internet page.
#
WINDOWS_BOOST_PATH= ARGUMENTS.get("windows_boost_path","")

# Prebuilt freetype2, cairo, cairomm etc.
#
WINDOWS_PREBUILT_PATH= ARGUMENTS.get("windows_prebuilt_path","")

env= Environment()

LINUX=  env["PLATFORM"]=="posix"
OSX=    env["PLATFORM"]=="darwin"
WINDOWS= env["PLATFORM"]=="win32"

# SCons does not pass env.vars automatically through to executing commands.
# On Windows, we want it to get them all (Visual C++ 2008).
#
if WINDOWS:
    env.Replace( ENV= os.environ )
    env.Append( CPPDEFINES= ["IMAGINE_FORMAT_PNG"] )  # no JPEG

env.Append( CPPPATH= [ "./include" ] )

if WINDOWS: 
    if env["CC"]=="cl":
        env.Append( CXXFLAGS= ["/EHsc"] )
else:
    env.Append( CPPDEFINES= ["UNIX"] )
    env.Append( CXXFLAGS= [
        # MAINFLAGS from orig. Makefile
        #
	"-std=c++0x",
	"-Wall",
	"-W",
        "-Wpointer-arith",
        "-Wcast-qual",
        "-Wwrite-strings",
        "-Wnon-virtual-dtor",
        "-Wno-pmf-conversions",
        "-Wsign-promo",
        "-Wchar-subscripts",
        "-Wredundant-decls",
        "-Woverloaded-virtual",
	"-std=c++0x",
        "-fPIC",
        "-Wall", 
	"-Wdeprecated",
        "-Wno-unused-parameter",
        "-Wno-variadic-macros",
	"-Wdeprecated"

	# "-Wno-deprecated", # hash_map
	    # DIFFICULTFLAGS from orig. Makefile (flags that cause some choking,
	    # would be good but not used)
	    #
	    #"-Weffc++",
	    #"-Winline",
	    #"-Wredundant-decls",
	    #"-Wshadow",
	    #"-Woverloaded-virtual",
	    #"-Wctor-dtor-privacy",
	    #"-Wold-style-cast",
	    #"-pedantic",
    ] )

BOOST_POSTFIX=""
BOOST_PREFIX=""

if WINDOWS:
    env.Append( CPPPATH= [ WINDOWS_BOOST_PATH ] )
    env.Append( LIBPATH= [ WINDOWS_BOOST_PATH + "/lib" ] )
    if DEBUG:
        BOOST_POSTFIX= "-vc90-mt-gd-1_35"
    else:
        BOOST_POSTFIX= "-vc90-mt-1_35"
        BOOST_PREFIX= "lib"
    env.Append( LIBS= [ BOOST_PREFIX+"boost_iostreams"+BOOST_POSTFIX,
                        BOOST_PREFIX+"boost_date_time"+BOOST_POSTFIX ] )

if WINDOWS:
    # Newbase from cvs local build
    #
    env.Append( CPPPATH= [ "../newbase/include" ] )
    env.Append( LIBPATH= [ "../newbase" ] )

elif LINUX:
    # Newbase from system install
    #
    env.Append( CPPPATH= [ PREFIX+"/include/smartmet/newbase" ] )

elif OSX:
    # Newbase from local CVS
    #
    env.Append( CPPPATH= [ "../newbase/include" ] )
    env.Append( LIBPATH= [ "../newbase" ] )

    # Fink
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
if WINDOWS:
    env.Append( CPPPATH= [ WINDOWS_PREBUILT_PATH+"/cairomm-1.6.4" ] )
    env.Append( LIBPATH= [ WINDOWS_PREBUILT_PATH+"/cairomm-1.6.4/MSVC_Net2005/cairomm/Release" ] )

    env.Append( CPPPATH= [ WINDOWS_PREBUILT_PATH+"/cairo-1.6.4/src" ] )
    #env.Append( LIBPATH= [ WINDOWS_PREBUILT_PATH+"/cairo-1.6.4/src/release" ] )
# else:
#   env.ParseConfig("pkg-config --cflags --libs cairomm-1.0") 

#
# Other libraries
#
if WINDOWS:
    # Yes, expanding LPNG creates two sets of folder levels (maybe a packaging bug in?)
    #
    env.Append( CPPPATH= [ WINDOWS_PREBUILT_PATH+"/lpng1231/lpng1231", 
                           WINDOWS_PREBUILT_PATH+"/zlib123" ] )
else:
    env.Append( LIBS= [ "jpeg", "png", "z" ] )


#
# Debug settings
#
if DEBUG:
    if WINDOWS:
        if env["CC"]=="cl":
            env.AppendUnique( CPPDEFINES=["_DEBUG","DEBUG"] )
            # Debug multithreaded DLL runtime, no opt.
            env.AppendUnique( CCFLAGS=["/MDd", "/Od"] )
            # Each obj gets own .PDB so parallel building (-jN) works
            env.AppendUnique( CCFLAGS=["/Zi", "/Fd${TARGET}.pdb"] )
    else:
        env.Append( CXXFLAGS=[ "-O0", "-g", "-Werror",
    
            # EXTRAFLAGS from orig. Makefile (for 'debug' target)
            #
	    "-Werror",
            "-ansi",
            "-Wcast-qual",
            "-Winline",
            "-Wno-multichar",
            "-Wno-pmf-conversions",
            "-Woverloaded-virtual",
            "-Wpointer-arith",
            "-Wredundant-decls",
            "-Wsign-promo",
            "-Wwrite-strings",
	    #         "-Wconversion",
        ] )

#
# Release settings
#
if RELEASE or PROFILE:
    if WINDOWS:
        if env["CC"]=="cl":
            # multithreaded DLL runtime, reasonable opt.
            env.AppendUnique( CCFLAGS=["/MD", "/Ox"] )
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

if WINDOWS:
    for fn in Glob("source/*.cpp"): 
        s= os.path.basename( str(fn) )
        obj_s= OBJDIR+"/"+ s.replace(".cpp","")

        objs += env.Object( obj_s, fn )
        shared_objs += env.SharedObject( obj_s, fn ) 
else:
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

if IMAGINE_USAGE:
    env.Append( CPPDEFINES= ["IMAGINE_USAGE"] )

#
# Make just the static lib (at least it should be default for just 'scons')
#
out= env.Library( "smartmet_imagine", objs )

if WINDOWS:
    Depends( out, "../newbase/smartmet_newbase.lib" )
elif LINUX:
    Depends( out, LIBDIR+"/libsmartmet_newbase.a" )
