// ======================================================================
//
// Definition of a PostScript style path element, with a ghostline extension.
//
// There is no respective NFmiPathElement.cpp file, all tasks needed so far
// are inlined here.
//
// History:
//
// 16.07.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================

#ifndef _NFMIPATHELEMENT_H
#define _NFMIPATHELEMENT_H

// ----------------------------------------------------------------------
// Path operations
// ----------------------------------------------------------------------

enum NFmiPathOperation { kFmiMoveTo,
						 kFmiLineTo,
						 kFmiGhostLineTo,
						 kFmiConicTo,
						 kFmiCubicTo };

// ----------------------------------------------------------------------
// A class defining a path element
// ----------------------------------------------------------------------

class NFmiPathElement
{
public:
  
  // Constructor
  
  NFmiPathElement(NFmiPathOperation operation, float x, float y)
    : itsOperation(operation), itsX(x), itsY(y)
  {}
  
  // Data-access
  
  NFmiPathOperation Oper(void) const	{ return itsOperation; }
  float X(void) const			{ return itsX; }
  float Y(void) const			{ return itsY; }
  
  // Setting data
  
  void Oper(NFmiPathOperation theOp)	{ itsOperation = theOp; }
  void X(float theX)			{ itsX = theX; }
  void Y(float theY)			{ itsY = theY; }
  
  // Equality comparison
  
  bool operator==(const NFmiPathElement & theElement) const
  {
    return (itsOperation == theElement.itsOperation &&
			itsX == theElement.itsX &&
			itsY == theElement.itsY);
  }
  
private:
  
  // Protect from bad constructors
  
  NFmiPathElement(void) {}
  
  // Data-members
  
  NFmiPathOperation	itsOperation;
  float			itsX;
  float			itsY;
};

#endif	// _NFMIPATHELEMENT_H

// ======================================================================
