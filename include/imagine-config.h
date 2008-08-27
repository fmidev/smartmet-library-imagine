/*
* imagine-config.h
*
* Defines whether Cairo or non-cairo (self made) rendering is used.
*
* This is in a config file (and not Makefile) so that other projects using
* Imagine will get the headers right automatically.
*/
#ifndef _imagine_config_h
#define _imagine_config_h

#ifdef IMAGINE_WITH_CAIRO
# error "Do NOT define IMAGINE_WITH_CAIRO in a Makefile"
#endif

// Comment this out for original (self made) rendering
#define IMAGINE_WITH_CAIRO

#endif

