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

#ifndef IMAGINE_NFMIPATHELEMENT_H
#define IMAGINE_NFMIPATHELEMENT_H

#include "NFmiDef.h"

namespace Imagine
{

  // ----------------------------------------------------------------------
  // Path operations
  // ----------------------------------------------------------------------
  
  enum NFmiPathOperation
	{
	  kFmiMoveTo,
	  kFmiLineTo,
	  kFmiGhostLineTo,
	  kFmiConicTo,
	  kFmiCubicTo
	};

    /* The struct is basically the same, a _bit_ shorter... then old (below)
    */
  struct NFmiPathElement {
    enum NFmiPathOperation op;
    float x,y;
    
    NFmiPathElement( enum NFmiPathOperation op_, float x_, float y_ )
        : op(op_), x(x_), y(y_) {}

	bool operator==( const NFmiPathElement &other ) const {
	  return op == other.op && x==other.x && y==other.y;
   }
  };

} // namespace Imagine
  
#endif	// IMAGINE_NFMIPATHELEMENT_H
  
// ======================================================================
  
