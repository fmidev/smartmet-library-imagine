// ======================================================================
//
// Definition of a PostScript style path, with a ghostline extension.
//
// History:
//
//	12.08.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================
#ifdef WIN32
#pragma warning(disable : 4786) // removes VC++ compiler's 'identifier name length over 255 characters' warning
#endif

#include "NFmiPath.h"
#include "NFmiCounter.h"
#include "NFmiEsriBox.h"

#include <iostream>
#ifdef UNIX
#include <strstream>
#else
#include <sstream>
#endif
#include <algorithm>

// ----------------------------------------------------------------------
// Append a path using a line of desired type
// ----------------------------------------------------------------------

void NFmiPath::Add(const NFmiPath & thePath, bool fExact)
{
  // We cannot simply copy, since we wish to change the first
  // moveto in the path being appended into a lineto of the
  // type implied by fExact
  
  NFmiPathData::const_iterator iter = thePath.itsElements.begin();
  
  for( ; iter!= thePath.itsElements.end() ; ++iter)
    {
      if(iter == thePath.itsElements.begin())
		Add(fExact ? kFmiLineTo : kFmiGhostLineTo, iter->X(), iter->Y());
      else
		Add(*iter);
    }
}

// ----------------------------------------------------------------------
// Insert a path using a line of desired type
// This requires using a loop, using insert for the list
// as a whole would insert the operations in their current
// order, but we actually want to reverse the path.
// ----------------------------------------------------------------------

void NFmiPath::Insert(const NFmiPath & thePath, bool fExact)
{
  // And insert one by one to get correct inverse order
  
  NFmiPathData::const_iterator iter = thePath.itsElements.begin();
  
  bool first = true;
  for( ; iter!=thePath.itsElements.end(); ++iter)
    {
      if( (first&&fExact) || (!first && iter->Oper()==kFmiLineTo) )
		InsertLineTo(iter->X(),iter->Y());
      else
		InsertGhostLineTo(iter->X(),iter->Y());
      first = false;
    }
}

// ----------------------------------------------------------------------
// Append a reversed path using a line of desired type
// This is tricky, since the correct operation for
// a point must be taken from the adjacent element.
//
// First point to be appended gets the type of the edge
// Second point to be appended gets the type of the first point appended
//
// Note that the first point to be appended is at the end of the
// list, which means we must use reverse iterators.
// ----------------------------------------------------------------------

void NFmiPath::AddReverse(const NFmiPath & thePath, bool fExact)
{
  // Holder for the next op to be added
  
  NFmiPathOperation op = (fExact ? kFmiLineTo : kFmiGhostLineTo);
  
  // Iterate
  
  NFmiPathData::const_reverse_iterator iter = thePath.itsElements.rbegin();
  for( ; iter!=thePath.itsElements.rend(); ++iter)
    {
      Add(op, iter->X(), iter->Y());
      op = iter->Oper();
    }
}

// ----------------------------------------------------------------------
// Close the last subpath in the path with the given type of line operation.
// (kFmiLineTo or kFmiGhostLineTo).
// Algorithm: Find the last kFmiMoveTo coordinate, draw line to it
// ----------------------------------------------------------------------

void NFmiPath::DoCloseLineTo(NFmiPathOperation theOper)
{
  NFmiPathData::reverse_iterator iter;
  for(iter=itsElements.rbegin(); iter!=itsElements.rend(); ++iter)
    {
      if(iter->Oper()==kFmiMoveTo)
		{
		  // The element to be added
		  
		  NFmiPathElement tmp(theOper,iter->X(),iter->Y());
		  
		  // Don't add if the last element is exactly the same
		  
		  if(!(itsElements.back()==tmp))
			Add(tmp);
		  break;
		}
    }
}

// ----------------------------------------------------------------------
// Append a path in string format.
//
// The expected format is
//
//      <operator> <x> <y>
//
// repeated over and over. The recognized operators are
//
//	M = moveto		m = relative moveto
//	L = lineto		l = relative lineto
//	G = ghostlineto		g = relative lineto
//	Q = conicto		q = relative conicto
//	C = cubicto		c = relative cubicto
//
// If the operator is omitted, the previous operator is assumed
// to be repeated. The previous operator is initialized to be
// a moveto before any parsing occurs.
//
// The following are valid examples which represents the same rectangle
//
//   M10,10 l-20,0 0,-20 20,0 0,20
//   M10,10 L-10,10 -10,-10 10,-10 10,10
//   M10,10 L-10,10 L-10,-10 L10,-10 L10,10
//
// Anything after a '#' character is ignored until a newline character
// ----------------------------------------------------------------------

using namespace std;

void NFmiPath::Add(const string & theString)
{
  NFmiPathOperation previous_op = kFmiMoveTo;
  float previous_x = 0;
  float previous_y = 0;
  bool previous_relative = false;
  
  unsigned int pos = 0;		// parser position
  
  int phase = 0;	// 0 = op, 1 = x, 2 = y
  
  float x = 0.0f;
  float y = 0.0f;
  bool relative = false;
  NFmiPathOperation op = kFmiMoveTo;
  
  while(pos<theString.size())
    {
      // Skip leading whitespace
	  
      if(isspace(theString[pos]))
		{
		  pos++;
		  continue;
		}
	  
      // Skip comments
	  
      if(theString[pos]=='#')
		{
		  while(pos<theString.size() && theString[pos]!='\n')
			pos++;
		  continue;
		}
	  
      // Parse operator
	  
      if(phase==0)
		{
		  // Initialize operator to be the previous one
		  
		  relative = previous_relative;
		  op = previous_op;
		  
		  // Assume there is an operator to be found
		  
		  char ch = theString[pos++];
		  
		  switch(toupper(ch))
			{
			case 'L':
			  op = kFmiLineTo;
			  relative = (ch!=toupper(ch));
			  break;
			case 'G':
			  op = kFmiGhostLineTo;
			  relative = (ch!=toupper(ch));
			  break;
			case 'M':
			  op = kFmiMoveTo;
			  relative = (ch!=toupper(ch));
			  break;
			case 'Q':
			  op = kFmiConicTo;
			  relative = (ch!=toupper(ch));
			  break;
			case 'C':
			  op = kFmiCubicTo;
			  relative = (ch!=toupper(ch));
			  break;
			default:
			  pos--;		// correct backwards - there was no operator
			}
		}
	  
      // Parse coordinates
	  
      else
		{
		  // Expecting some float, either x or y
		  
		  string asciinumber;
		  while(pos<theString.size() && !isspace(theString[pos]) && theString[pos]!=',')
			asciinumber += theString[pos++];
		  
		  // Skip the possible comma we may have found
		  
		  if(theString[pos]==',')
			pos++;
		  
		  float number = atof(asciinumber.c_str());
		  
		  if(phase==1)
			x = number;
		  else
			y = number;
		}
	  
      // Output movement
	  
      if(phase==2)
		{
		  if(relative)
			{
			  x += previous_x;
			  y += previous_y;
			}
		  Add(op,x,y);
		  previous_op = op;
		  previous_x = x;
		  previous_y = y;
		  previous_relative = relative;
		}
      phase = (phase+1)%3;
	  
    }
}

// ----------------------------------------------------------------------
// Simplify a path using the Douglas-Peucker algorithm.
// The input specifies the maximum allowed error in the recursive
// algorithm for deciding when to discard a point. A zero value
// is allowed, and means that no error is allowed. Simplifications
// may however occur, since we may have two moves which form a
// straight line, and hence can be simplified into a single move.
//
// Note that a sequence of moves can be simplified if and only
// if the type of the simplified movement is the same as of the
// original more complicated movement. That is, if a line changes
// into a ghostline, we must simplify the line part and the ghostline
// parts separately. Similarly a moveto *always* acts as a separator
// of different paths to be simplified.
// ----------------------------------------------------------------------

void NFmiPath::Simplify(float epsilon)
{
  if(epsilon<0.0)
    return;
  
  // Count the important points
  
  NFmiCounter<pair<float,float> > counter;
  
  NFmiPathData::const_iterator iter;
  for(iter=Elements().begin() ; iter!=Elements().end(); ++iter)
    {
      switch(iter->Oper())
		{
		case kFmiMoveTo:
		case kFmiLineTo:
		case kFmiGhostLineTo:
		  counter.Add(make_pair(iter->X(),iter->Y()));
		  break;
		default:
		  break;
		}
    }
  
  // Then simplify all linear segments of equal type
  //
  // a) Optional one moveto + lineto's
  // b) Optional one moveto + ghostlineto's
  
  
  
  
}

// ----------------------------------------------------------------------
// Translate the path by the given amount
// ----------------------------------------------------------------------

void NFmiPath::Translate(float theX, float theY)
{
  NFmiPathData::iterator iter;
  
  for(iter=itsElements.begin(); iter!=itsElements.end(); ++iter)
    {
      iter->X(iter->X()+theX);	// X += theX
      iter->Y(iter->Y()+theY);	// Y += theY
    }
}

// ----------------------------------------------------------------------
// Scale the path by the given amount
// ----------------------------------------------------------------------

void NFmiPath::Scale(float theScale)
{
  NFmiPathData::iterator iter;
  
  for(iter=itsElements.begin(); iter!=itsElements.end(); ++iter)
    {
      iter->X(iter->X()*theScale);
      iter->Y(iter->Y()*theScale);
    }
}

// ----------------------------------------------------------------------
// Rotate the path by the given amount
// ----------------------------------------------------------------------

void NFmiPath::Rotate(float theAngle)
{
  const float pi = 3.14159265358979f;
  
  NFmiPathData::iterator iter;
  
  float cosa = cos(theAngle*pi/180);
  float sina = sin(theAngle*pi/180);
  
  for(iter=itsElements.begin(); iter!=itsElements.end(); ++iter)
    {
      float x = iter->X();
      float y = iter->Y();
      iter->X(x*cosa+y*sina);
      iter->Y(-x*sina+y*cosa);
    }
}

// ----------------------------------------------------------------------
// Apply affine transformation on the path
// ----------------------------------------------------------------------

void NFmiPath::Transform(NFmiAffine & theAffine)
{
  NFmiPathData::iterator iter;
  
  for(iter=itsElements.begin(); iter!=itsElements.end(); ++iter)
    {
      float x = iter->X();
      float y = iter->Y();
      iter->X(theAffine.X(x,y));
      iter->Y(theAffine.Y(x,y));
    }
}

// ----------------------------------------------------------------------
// Apply alignment on the path
// ----------------------------------------------------------------------

void NFmiPath::Align(NFmiAlignment theAlignment, float theX, float theY)
{
  NFmiEsriBox box(BoundingBox());
  
  double xfactor = XAlignmentFactor(theAlignment);
  double yfactor = YAlignmentFactor(theAlignment);
  
  Translate(theX - (box.Xmin()*(1-xfactor)+box.Xmax()*xfactor),
			theY - (box.Ymin()*(1-yfactor)+box.Ymax()*yfactor));
}

// ----------------------------------------------------------------------
// Apply a projection to a path
// ----------------------------------------------------------------------

void NFmiPath::Project(NFmiArea * theArea)
{
  if(theArea!=0)
    {
      NFmiPathData::iterator iter;
      for(iter=itsElements.begin(); iter!=itsElements.end(); ++iter)
		{
		  NFmiPoint pt = theArea->ToXY(NFmiPoint(iter->X(),iter->Y()));
		  iter->X(pt.X());
		  iter->Y(pt.Y());
		}
    }
}

// ----------------------------------------------------------------------
// Return the bounding box of the path.
// Note: Bezier curve bounding boxes not implemented yet
// ----------------------------------------------------------------------

NFmiEsriBox NFmiPath::BoundingBox(void) const
{
  NFmiEsriBox box;
  
  NFmiPathData::const_iterator iter;
  
  for(iter=itsElements.begin(); iter!=itsElements.end(); ++iter)
    {
      switch(iter->Oper())
		{
		case kFmiMoveTo:
		case kFmiLineTo:
		case kFmiGhostLineTo:
		  box.Update(iter->X(),iter->Y());
		  break;
		case kFmiConicTo:
		  break;
		case kFmiCubicTo:
		  break;
		}
    }
  
  return box;
}


// ----------------------------------------------------------------------
// Simplify by joining colinear consecutive line segments into
// one longer line segment, provided the points are monotonously
// ordered.
//
// Note: While examining any coordinate, we add the fixed offset
//       given by the user to any value. In particular, when
//       the coordinates are expected to be say from 0 to 400,
//       it may happen that due to rounding errors coordinates
//       near zero might not be considered colinear. Using
//       offset=1 should in general fix the problem. Using
//	 offset=0 will use the exact original coordinates.
//
// This simplification ignores possible important points.
// ----------------------------------------------------------------------

void NFmiPath::SimplifyLines(float theOffset)
{
  NFmiPathData::iterator iter = itsElements.begin();
  NFmiPathData::iterator end  = itsElements.end();
  
  NFmiPathData::iterator deliter;	// used when deleting
  
  // Cached previous path element:
  
  NFmiPathOperation oper1,oper2,oper3;
  float x1,y1,x2,y2,x3,y3;
  
  oper1 = oper2 = oper3 = kFmiMoveTo;
  x1 = y1 = x2 = y2 = x3 = y3 = kFloatMissing;
  
  int cachesize = 0;
  
  while(iter!=end)
    {
      // Shift old data backwards
	  
      oper1=oper2; oper2=oper3;
      x1=x2; x2=x3;
      y1=y2; y2=y3;
	  
      // Establish new data
	  
      oper3 = iter->Oper();
      x3 = theOffset+iter->X();
      y3 = theOffset+iter->Y();
	  
      // Delete-iterator is current element, 
	  
      ++iter;
	  
      // Get atleast 3 points before doing anything
	  
	  //      cachesize = min(3,cachesize+1);
      cachesize = std::min(3,cachesize+1);
	  
      if(cachesize <3)
		continue;
	  
      // Now, if the last 2 operations are of equal lineto-type,
      // we may simplify the sequence of 3 points:
      //
      // A--B--C becomes A---C if B is somewhere on the
      // line connecting A and C.
	  
      if(oper2==oper3 && (oper2==kFmiLineTo || oper2==kFmiGhostLineTo))
		{
		  // The line cannot be straight unless it is monotonous
		  
		  if(x1<x2 && x2>x3) continue;
		  if(x1>x2 && x2<x3) continue;
		  if(y1<y2 && y2>y3) continue;
		  if(y1>y2 && y2<y3) continue;
		  
		  
		  // Vertical and horizontal lines are easily tested
		  // General case lines are compared based on their
		  // angles: dy/dx must be equal so that
		  //       (y2-y1)/(x2-x1) == (y3-y2)/(x3-x2)
		  // <==>  (y2-y1)(x3-x2) == (y3-y2)(x2-x1)
		  // We do not allow for rounding errors in the test,
		  // it's probably not worth the trouble.
		  
		  if ( (x1==x2 && x2==x3) || (y1==y2 && y2==y3) ||
			   ((x3-x1)*(y2-y1)-(y3-y1)*(x2-x1)==0.0) )
			{
			  oper2 = oper1;
			  x2=x1;
			  y2=y1;
			  cachesize--;
			  
			  deliter = iter;			// ABC->D
			  --deliter;			// AB->CD
			  --deliter;			// A->BCD
			  itsElements.erase(deliter);	// A->CD
			}
		}
    }
}


// ----------------------------------------------------------------------
// Output path in textual form
// ----------------------------------------------------------------------

std::ostream& operator<< (std::ostream& os,const NFmiPath & thePath)
{
  NFmiPathData::const_iterator iter = thePath.Elements().begin();
  for( ; iter!=thePath.Elements().end(); ++iter)
    {
      // Special code for first move
	  
      if(iter != thePath.Elements().begin())
		os << " ";
	  
      if(iter->Oper() == kFmiMoveTo)
		os << 'M';
      else if(iter->Oper() == kFmiLineTo)
		os << 'L';
      else if(iter->Oper() == kFmiGhostLineTo)
		os << 'G';
      else if(iter->Oper() == kFmiConicTo)
		os << 'Q';
      else if(iter->Oper() == kFmiCubicTo)
		os << 'C';
      else
		os << '?';
	  
      os << iter->X() << "," << iter->Y();
	  
    }
  return os;
}

// ----------------------------------------------------------------------
// Return a SVG-string representation of the path
// If relative_moves=true, relative movements are preferred over
// absolute moves. This usually generates shorter SVG.
// ----------------------------------------------------------------------

string NFmiPath::SVG(bool relative_moves) const
{
  // Note: Do NOT use output string streams, atleast not with
  //       g++ v2.92. The implementation is broken, and will
  //       mess up the output string. Instead we reserve a reasonable
  //       amount of space for the string from the beginning in the
  //       hope that output will be reasonably fast.
  
  string os;
  
  float last_x, last_y;
  NFmiPathOperation last_op = kFmiMoveTo;
  
  last_x = last_y = kFloatMissing;
  
  NFmiPathData::const_iterator iter = Elements().begin();
  for( ; iter!=Elements().end(); ++iter)
    {
      // Special code for first move
	  
      if(iter->Oper()==kFmiConicTo || iter->Oper()==kFmiCubicTo)
		{
		  cerr << "Error: Conic and Cubic control points not supported in NFmiPath::SVG() yet" << endl;
		  exit(1);
		}
	  
      if(iter==Elements().begin())
		{
		  os += (relative_moves ? "m" : "M")
			+ ftoa(iter->X()) + "," + ftoa(iter->Y());
		}
	  
      // Relative moves are "m dx dy" and "l dx dy"
      else if(relative_moves)
		{
		  if(iter->Oper() == kFmiMoveTo)
			os += (last_op==kFmiMoveTo ? " " : " m");
		  else if(iter->Oper() == kFmiLineTo)
			os += ( (last_op==kFmiLineTo||last_op==kFmiGhostLineTo) ? " " : " l");
		  os += ftoa((iter->X()-last_x)) + "," + ftoa((iter->Y()-last_y));
		}
	  
      // Absolute moves are "M x y" and "L x y"
      else
		{
		  if(iter->Oper() == kFmiMoveTo)
			os += (last_op==kFmiMoveTo ? " " : " M");
		  else
			os += ((last_op==kFmiLineTo||last_op==kFmiGhostLineTo) ? " " : " L");
		  os += ftoa(iter->X()) + "," + ftoa(iter->Y());
		}
	  
      last_op = iter->Oper();
	  
      if(relative_moves)
		{
		  last_x = iter->X();
		  last_y = iter->Y();
		}
	  
    }
  return os;
}

// ----------------------------------------------------------------------
// Convert floating point number to string for the benefit of SVG() method
// ----------------------------------------------------------------------

string NFmiPath::ftoa(float theValue) const
{
  char msg[30];
  int nchars = sprintf(msg,"%g",theValue);
  assert(nchars>0);
  string str(msg,nchars);
  return str;
}

// ----------------------------------------------------------------------
// Add the path to a fill map
// ----------------------------------------------------------------------

void NFmiPath::Add(NFmiFillMap & theMap) const
{
  // Data holders for moves. 1 is the newest, 4 the oldest
  
  NFmiPathOperation op1 = kFmiMoveTo;
  NFmiPathOperation op2 = kFmiMoveTo;
  NFmiPathOperation op3 = kFmiMoveTo;
  NFmiPathOperation op4 = kFmiMoveTo;
  float x1 = kFloatMissing;
  float x2 = kFloatMissing;
  float x3 = kFloatMissing;
  float x4 = kFloatMissing;
  float y1 = kFloatMissing;
  float y2 = kFloatMissing;
  float y3 = kFloatMissing;
  float y4 = kFloatMissing;
  
  // The iterator for traversing the data
  
  NFmiPathData::const_iterator iter = Elements().begin();
  
  for( ; iter!=Elements().end(); ++iter)
    {
      x1 = iter->X();
      y1 = iter->Y();
      op1 = iter->Oper();
      
      switch(iter->Oper())
		{
		case kFmiMoveTo:
		  break;
		  
		case kFmiLineTo:
		case kFmiGhostLineTo:
		  {
			switch(op2)
			  {
			  case kFmiConicTo:
				theMap.AddConic(x3,y3,x2,y1,x1,y1);	// Conic segment
				break;
			  case kFmiCubicTo:
				if(op3==kFmiCubicTo)
				  theMap.AddCubic(x4,y4,x4,y3,x2,y2,x1,y1);
				break;
			  default:
				theMap.Add(x2,y2,x1,y1);	// Line segment
				break;
			  }
		  }
		  
		case kFmiConicTo:
		  break;
		case kFmiCubicTo:
		  break;
		}
	  
      // Update movement history
	  
      op4=op3; op3=op2; op2=op1;
      x4=x3; x3=x2; x2=x1;
      y4=y3; y3=y2; y2=y1;
	  
    }
}

// ----------------------------------------------------------------------
// Stroke onto given image using various Porter-Duff rules
// ----------------------------------------------------------------------

void NFmiPath::Stroke(NFmiImage & theImage,
					  NFmiColorTools::Color theColor,
					  NFmiColorTools::NFmiBlendRule theRule) const
{
  // Quick exit if color is not real
  
  if(theColor==NFmiColorTools::NoColor)
    return;
  
  // Current point is not defined yet
  
  float lastX = kFloatMissing;
  float lastY = kFloatMissing;
  
  NFmiPathData::const_iterator iter = Elements().begin();
  
  for( ; iter!= Elements().end() ; ++iter)
    {
      // Next point
	  
      float nextX = iter->X();
      float nextY = iter->Y();
	  
      if(iter->Oper()==kFmiConicTo || iter->Oper()==kFmiCubicTo)
		{
		  cerr << "Error: Conic/Cubic control points not supported in NFmiPath::Stroke()" << endl;
		  exit(1);
		}
	  
      // Only LineTo operations get rendered
	  
      if(iter->Oper() == kFmiLineTo)
		if(lastX!=kFloatMissing && lastY!=kFloatMissing &&
		   nextX!=kFloatMissing && nextY!=kFloatMissing)
		  {
			theImage.StrokeBasic(lastX,lastY,nextX,nextY,theColor,theRule);
		  }
      
      // New last point
	  
      lastX = nextX;
      lastY = nextY;
    }
  
}

// ----------------------------------------------------------------------
// Return a wide version of the input path. This is used when stroking
// a wide line, but may be useful for other purposes too.
//
// Algorithm:
//
//	If MoveTo x,y
//		Close previous possibly open segment
//	If LineTo x,y
//		If there is no active open segment
//			Begin one from lastX,lastY to x,y
//		Else
//			Append path from lastX,lastY to x,y
//	If GhostLineTo x,y
//		Close previous possibly open segment
//
// Note:
//
// Given a line from x1,y1 to x2,y2 the widened coordinates are
// obtained with
//
//	DX = (x2-x1)
//	DY = (y2-y1)
//	DF = w/2/sqrt(DX^2+DY^2)
//
//	dx = -DF*DY
//	dy = DF*DX
//
// and the coordinates are
//
//	x1 +- dx	x2 +- dx
//	y1 +- dy	y2 +- dy
//
// ----------------------------------------------------------------------

// UNFINISHED UNFINISHED UNFINISHED UNFINISHED UNFINISHED

NFmiPath NFmiPath::StrokePath(float theWidth) const
{
  
  // The path being generated
  
  NFmiPath outpath;
  
  // The current open subsegment
  
  NFmiPath segment;
  
  // Status variables
  
  float firstX,firstY;
  float lastX,lastY;
  
  firstX = firstY = lastX = lastY = kFloatMissing;
  
  
  // Iterate through the data
  
  NFmiPathData::const_iterator iter;
  
  for(iter=Elements().begin(); iter!=Elements().end(); ++iter)
    {
      float x = iter->X();
      float y = iter->Y();
      NFmiPathOperation op = iter->Oper();
	  
      switch(op)
		{
		case kFmiConicTo:
		case kFmiCubicTo:
		  // Unfinished
		  break;
		  
		case kFmiMoveTo:
		case kFmiGhostLineTo:
		  {
			// If there is an unfinished segment, we must close it now.
			
			if(!segment.Elements().empty())
			  segment.CloseLineTo();
			
			// Append it to the outpath
			
			outpath.Add(segment);
			
			// And begin a new segment
			
			segment.Clear();
			break;
		  }
		  
		case kFmiLineTo:
		  {
			// Calculate widened line offsets
			
			float DX = x-lastX;
			float DY = y-lastY;
			float DF = theWidth/2/sqrt(DX*DX+DY*DY);
			
			float dx = -DF*DY;
			float dy = DF*DX;
			
			// If haven't started yet, start
			
			if(segment.Elements().empty())
			  {
				firstX = x;
				firstY = y;
				outpath.MoveTo(lastX+dx,lastY+dy);
				outpath.LineTo(lastX-dx,lastY-dy);
				outpath.LineTo(x-dx,y-dy);
				outpath.InsertLineTo(x+dx,y+dy);
			  }
			
			// Otherwise recalculate the current open path
			
			else
			  {
			  }
			
		  }
		}
      lastX = x;
      lastY = y;
	  
    }
  
  // If we were left with a open segment, close it and append it too
  
  if(!segment.Elements().empty())
    {
      segment.CloseLineTo();
      outpath.Add(segment);
    }
  
  return outpath;
}

// ======================================================================


