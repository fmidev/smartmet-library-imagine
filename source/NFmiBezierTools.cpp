// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace Imagine::NFmiBezierTools
 */
// ======================================================================
/*!
 * \namespace Imagine::NFmiBezierTools
 *
 * \brief Utility subroutines for various Bezier calculations
 *
 * This namespace is not intended to be used outside the library,
 * instead here are some common utility subroutines for various
 * Bezier fitting functions such as those in
 * Imagine::NFmiCardinalBezierFit and Imagine::NFmiApproximateBezierFit.
 *
 */
// ======================================================================

#include "NFmiBezierTools.h"
#include "NFmiPath.h"

#include <utility>

namespace Imagine
{
  namespace NFmiBezierTools
  {
	// ----------------------------------------------------------------------
	/*!
	 * \brief Test if the given regular segment is closed
	 *
	 * A regular segment is a path starting with a moveto and followed
	 * by lineto elements only.
	 *
	 * \param thePath The path to test
	 * \return True if the segment is closed
	 */
	// ----------------------------------------------------------------------

	bool IsClosed(const NFmiPath & thePath)
	{
	  if(thePath.Size() < 2)
		return false;
	  
	  const NFmiPathElement & first = thePath.Elements().front();
	  const NFmiPathElement & last = thePath.Elements().back();
	  
	  return (first.X() == last.X() && first.Y() == last.Y());
			  
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Split the given path into regular line and other segments
	 *
	 * A regular line segment is considered to be one with
	 * a leading moveto command followed by a number of lineto
	 * commands, atleast two of them. Zero length lineto commands
	 * will be automatically removed to avoid problems during
	 * Bezier fitting.
	 *
	 * \param thePath The path to be splitted
	 * \return List of subpaths
	 */
	// ----------------------------------------------------------------------

	Segments SplitSegments(const NFmiPath & thePath)
	{
	  Segments out;

	  bool isregular = false;
	  NFmiPath outpath;

	  for(NFmiPathData::const_iterator it = thePath.Elements().begin();
		  it != thePath.Elements().end();
		  ++it)
		{
		  // current outpath must be flushed if
		  // a) operator is moveto
		  // b) type changes from regular to nonregular

		  bool flush = (!outpath.Empty() &&
						((*it).Oper() == kFmiMoveTo ||
						((*it).Oper() == kFmiLineTo && !isregular)));

		  

		  if(flush)
			{
			  // must have sufficiently many lineto cmds to be regular
			  if(outpath.Size() <= 3) isregular = false;
			  // flush
			  out.push_back(std::make_pair(outpath,isregular));
			  outpath.Clear();
			}

		  // add the element, but remove zero-length line segments
		  switch((*it).Oper())
			{
			case kFmiMoveTo:
			case kFmiConicTo:
			case kFmiCubicTo:
			  outpath.Add(*it);
			  break;
			case kFmiGhostLineTo:
			case kFmiLineTo:
			  {
				if(outpath.Empty())
				  outpath.Add(*it);
				else if(outpath.Elements().back().X() != (*it).X() ||
						outpath.Elements().back().Y() != (*it).Y())
				  outpath.Add(*it);
				  
			  }
			}

		  // lineto cannot start a new regular segment!
		  if(outpath.Size() == 1)
			isregular = ((*it).Oper() == kFmiMoveTo);

		}

	  if(!outpath.Empty())
		out.push_back(std::make_pair(outpath,isregular));

	  return out;

	}

  } // namespace NFmiBezierTools

} // namespace Imagine
