// ======================================================================
//
// Constants related to various font types.
//
// Instead of scattering path, suffix etc info to low level font drivers,
// the constants are collected here for easy modification, inclusion and
// comparison.
//
// The constants here are intended to be used by low level font drivers
// only. Put user level constants such as font type enums into NFmiFont.h.
//
// History:
//
// 20.11.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================

#ifndef _NFMIFONTGLOBALS_H
#define _NFMIFONTGLOBALS_H

#include <string>

// We hardcode the DPI resolution to 96, since
//
//   a) We want the output to be independent of the drawing platform,
//      hence the system is not queried for the proper value.
//
//   b) Some TrueType fonts are apparently optimized for 96 DPI, since
//      its is the default resolution of Windows. Hence we may get
//      better looking fonts.

const int kFmiDisplayResolution = 96;

const std::string kFmiSuffixHershey(".jhf");
const std::string kFmiSuffixTrueType(".ttf");
const std::string kFmiSuffixType1(".pfb");

#endif // _NFMIFONTFLOBALS_H

// ======================================================================
