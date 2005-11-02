// ======================================================================
//
// Definition of a PostScript style path, with a ghostline extension.
//
// Notes on conic control points
// -----------------------------
//
// This defines a regular conic segment where x2,y2 is the control point
// when moving from x1,y1 to x3,y3
//
//   <M|L|G> x1 y1 <Q> x2 y2 <L|G> x3 y3
//
// The following defines a shorthand conic segment where x2,y2 is the
// control point when moving from (x1+x2)/2,(y1+y2)/2 to x3,y3
// to
//
//   <Q> x1 y1 <Q> x2 y2 <L|G> x3 y3
//
//   <Q> x1 y1 <Q> x2 y2 <Q> x3 y3
//
//
//
// History:
//
// 16.07.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================

#ifndef IMAGINE_NFMIPATH_H
#define IMAGINE_NFMIPATH_H

// Essential includes

#include "NFmiPathElement.h"
#include "NFmiFillMap.h"
#include "NFmiAffine.h"
#include "NFmiArea.h"

#include <string>
#include <deque>
#include <algorithm>
#include <cmath>		// for sqrt
#include <cstdio>		// for sprintf in ftoa

#include <iostream>		// << is overloaded

class NFmiGrid;

namespace Imagine
{
  class NFmiEsriBox;
  
  typedef std::deque<NFmiPathElement> NFmiPathData;
  
  // ----------------------------------------------------------------------
  // A class defining a path
  // ----------------------------------------------------------------------
  
  class _FMI_DLL NFmiPath : public NFmiDrawable
  {
  public:
	
	virtual ~NFmiPath(void) {}
	
	NFmiPath() : itsInsideOut(false), itsElements() { }
	NFmiPath(const NFmiPathData &thePathData, bool theInsideOut = false) : itsInsideOut(theInsideOut), itsElements(thePathData) { }
	
	// Tulostusoperaattorin ylikuormitus
	
	friend std::ostream& operator<<(std::ostream& os, const NFmiPath & thePath);
	
	// Data-access
	
	const NFmiPathData & Elements(void) const { return itsElements; }
	
	int Size(void) const { return static_cast<int>(itsElements.size()); }
	int Empty(void) const { return itsElements.empty(); }
	
	// Clear contents
	
	void Clear(void)
	{ itsElements.clear(); itsInsideOut=false; }
	
	// Add the given path element
	
	void Add(const NFmiPathElement & theElement)
	{ itsElements.push_back(theElement); }
	
	// Add the given path element via its components
	
	void Add(NFmiPathOperation theOper, float theX, float theY)
	{ itsElements.push_back(NFmiPathElement(theOper,theX,theY)); }
	
	// Move to a new point
	
	void MoveTo(float theX, float theY)
	{ itsElements.push_back(NFmiPathElement(kFmiMoveTo,theX,theY)); }
	
	// Draw a line from current point to new point
	
	void LineTo(float theX, float theY)
	{ itsElements.push_back(NFmiPathElement(kFmiLineTo,theX,theY)); }
	
	// Draw an invisible line from current point to new point
	
	void GhostLineTo(float theX, float theY)
	{ itsElements.push_back(NFmiPathElement(kFmiGhostLineTo,theX,theY)); }
	
	// Draw a line from the given point to the first point
	
	void InsertLineTo(float theX, float theY)
	{
	  itsElements.front().Oper(kFmiLineTo);
	  itsElements.push_front(NFmiPathElement(kFmiMoveTo,theX,theY));
	}
	
	// Draw a line from the given point to the first point
	
	void InsertGhostLineTo(float theX, float theY)
	{
	  itsElements.front().Oper(kFmiGhostLineTo);
	  itsElements.push_front(NFmiPathElement(kFmiMoveTo,theX,theY));
	}
	
	// Add a conic control point
	
	void ConicTo(float theX, float theY)
	{
	  itsElements.push_back(NFmiPathElement(kFmiConicTo,theX,theY));
	}
	
	// Add a cubic control point
	
	void CubicTo(float theX, float theY)
	{
	  itsElements.push_back(NFmiPathElement(kFmiCubicTo,theX,theY));
	}
	
	// Close the last subpath with a line
	
	void CloseLineTo(void)
	{
	  DoCloseLineTo(kFmiLineTo);
	}
	
	// Close the path with an invisible line
	
	void CloseGhostLineTo(void)
	{
	  DoCloseLineTo(kFmiGhostLineTo);
	}
	
	// Append a path without a joining line. The first moveto
	// may be omitted if the coordinate is the same as the
	// endpoint of the current path. This is a required feature
	// for some path simplification algorithms.
	
	void Add(const NFmiPath & thePath)
	{
	  if(thePath.Empty())
		return;

	  NFmiPathData::const_iterator it = thePath.itsElements.begin();
	  // strip leading moveto if the coordinate is the same as last end point
	  if(!Empty() &&
		 (*it).Oper() == kFmiMoveTo &&
		 (*it).X() == itsElements.back().X() &&
		 (*it).Y() == itsElements.back().Y())
		++it;

	  itsElements.insert(itsElements.end(), it, thePath.itsElements.end());
	}
	
	// Append a path using a line of desired type
	
	void Add(const NFmiPath & thePath, bool fExact);
	
	// Insert a path using a line of desired type
	
	void Insert(const NFmiPath & thePath, bool fExact);
	
	// Append a reversed path using a line of desired type
	
	void AddReverse(const NFmiPath & thePath, bool fExact);
	
	// Simplify using Douglas-Peucker algorithm. The input is the
	// maximum allowed error for any line segment.
	
	void Simplify(float epsilon=0.0);
	
	// Simplify long straight line segments
	
	void SimplifyLines(float offset=0.0);
	
	// Return SVG-string description
	
	std::string SVG(bool relative_moves=false, bool removeghostlines=true) const;
	
	// Add the path to a fill map
	
	void Add(NFmiFillMap & theMap) const;
	
	// Add a string representation of a path to the path
	
	void Add(const std::string & theString);
	
	// Rotate the path by the given decimal degrees
	
	void Rotate(float theAngle);
	
	// Translate the path by the given amount
	
	void Translate(float theX, float theY);
	
	// Scale the path by the given amount
	
	void Scale(float theScale);
	
	// Scale the path by the given amounts in x- and y-directions
	
	void Scale(float theXScale, float theYScale);
	
	// Apply a general affine transformation
	
	void Transform(NFmiAffine & theAffine);
	
	// Align path to desired corner
	
	void Align(NFmiAlignment theAlignment, float theX=0.0, float theY=0.0);
	
	// Project
	
	void Project(const NFmiArea * const theArea);
	
	void InvProject(const NFmiArea * const theArea);
	
	void InvGrid(const NFmiGrid * const theGrid);
	
	// Stroke onto given image using various Porter-Duff rules
	
	void Stroke(NFmiImage & theImage,
				NFmiColorTools::Color theColor,
				NFmiColorTools::NFmiBlendRule theRule=NFmiColorTools::kFmiColorCopy) const;
	
	// Return the bounding box
	
	const NFmiEsriBox BoundingBox(void) const;
	
	const NFmiPath Clip(double theX1, double theY1, double theX2, double theY2, double theMargin = 0) const;
	
	void InsideOut(void) { itsInsideOut = !itsInsideOut; }
	bool IsInsideOut() const { return itsInsideOut; }
	
  private:
	
	// Close the last subpath with an invisible or visible line
	
	void DoCloseLineTo(NFmiPathOperation theOper);
	
	// Floating point number to string
	// This is needed because stringstream version of SVG() does not
	// work using g++ v2.95, instead we must format the numbers by
	// ourselves for string concatenation operations
	
	std::string ftoa(float theValue) const;
	
	// Simplification subroutines
	
	void SimplifyTrivial(void);
	
	// Data-members
	
	bool itsInsideOut;
	NFmiPathData itsElements;
	
  };

} // namespace Imagine
  
#endif	// IMAGINE_NFMIPATH_H
  
// ----------------------------------------------------------------------
