// ======================================================================
//
// See documentation in NFmiEdge.cpp
//
// History:
//
// 12.08.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================

#ifndef _NFMIEDGE_H
#define _NFMIEDGE_H

#include <cmath>

#include "NFmiGlobals.h"	// for kFloatMissing

// ----------------------------------------------------------------------
// A working class, holding an unique edge
// ----------------------------------------------------------------------

class NFmiEdge
{
public:
  
  // The constructor will sort the vertices
  
  NFmiEdge(float x1,float y1, float x2, float y2,bool exact)
    : itsX1(x1), itsY1(y1), itsX2(x2), itsY2(y2), fExact(exact)
  {
    if(itsX2<itsX1 || (itsX2==itsX1 && itsY2<itsY1))
      {
		
		itsX1 = x2;
		itsX2 = x1;
		itsY1 = y2;
		itsY2 = y1;
      }
  };
  
  // Getting data values
  
  float X1(void) const		{ return itsX1; }
  float Y1(void) const		{ return itsY1; }
  float X2(void) const		{ return itsX2; }
  float Y2(void) const		{ return itsY2; }
  bool Exact(void) const	{ return fExact; }
  
  // Setting data values
  
  void X1(float theX)	{ itsX1 = theX; }
  void Y1(float theY)	{ itsY1 = theY; }
  void X2(float theX)	{ itsX2 = theX; }
  void Y2(float theY)	{ itsY2 = theY; }
  
  // Equality comparison for our own purposes and for set find methods.
  
  bool operator==(const NFmiEdge &theEdge) const
  {
    return (itsX1==theEdge.itsX1 &&
			itsY1==theEdge.itsY1 &&
			itsX2==theEdge.itsX2 &&
			itsY2==theEdge.itsY2 && 
			fExact==theEdge.fExact);
  }
  
  // Less-than comparison needed for set<NFmiEdge>
  
  bool operator<(const NFmiEdge &theEdge) const
  {
    if(itsX1!=theEdge.itsX1)
      return (itsX1<theEdge.itsX1);
    if(itsY1!=theEdge.itsY1)
      return (itsY1<theEdge.itsY1);
    if(itsX2!=theEdge.itsX2)
      return (itsX2<theEdge.itsX2);
    return (itsY2<theEdge.itsY2);
  }
  
private:
  
  // Protect from misuse
  
  NFmiEdge(void) {};
  
  // Data elements
  
  float itsX1;		// Start point coordinates
  float itsY1;
  float itsX2;		// End point coordinates
  float itsY2;
  bool  fExact;		// Is the edge exactly on a contour?
};

// A hash function intended for hash_set() etc

struct NFmiEdgeHasher
{
  size_t operator()(const NFmiEdge & theEdge) const
  {
    return ((static_cast<size_t>(theEdge.X1()*134631) << 0 )^
			(static_cast<size_t>(theEdge.Y1()*13123) << 3) ^
			(static_cast<size_t>(theEdge.X2()*124132) << 7) ^
			(static_cast<size_t>(theEdge.Y2()*439216) << 9) ^
			theEdge.Exact() << 20);
  }
};

// ----------------------------------------------------------------------

#endif // _NFMIEDGE_H
