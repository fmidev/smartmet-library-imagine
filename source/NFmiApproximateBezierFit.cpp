// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace Imagine::NFmiApproximateBezierFit
 */
// ======================================================================
/*!
 * \namespace Imagine::NFmiApproximateBezierFit
 *
 * \brief Calculating an approximate bezier fit
 *
 */
// ======================================================================

#include "NFmiApproximateBezierFit.h"
#include "NFmiBezierTools.h"
#include "NFmiPath.h"

#include <list>

using namespace std;

namespace Imagine
{

  namespace
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Estimate left tangent
	 *
	 * \param thePath The path from which to estimate
	 * \return The tangent estimate
	 */
	// ----------------------------------------------------------------------

	NFmiPoint ComputeLeftTangent(const NFmiPathData & thePath)
	{
	  const double dx = thePath[1].X() - thePath[0].X();
	  const double dy = thePath[1].Y() - thePath[0].Y();
	  const double len = sqrt(dx*dx+dy*dy);
	  if(len==0)
		return NFmiPoint(0,0);
	  else
		return NFmiPoint(dx/len,dy/len);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Estimate right tangent
	 *
	 * \param thePath The path from which to estimate
	 * \return The tangent estimate
	 */
	// ----------------------------------------------------------------------

	NFmiPoint ComputeRightTangent(const NFmiPathData & thePath)
	{
	  const int n = thePath.size();
	  const double dx = thePath[n-2].X() - thePath[n-1].X();
	  const double dy = thePath[n-2].Y() - thePath[n-1].Y();
	  const double len = sqrt(dx*dx+dy*dy);
	  if(len==0)
		return NFmiPoint(0,0);
	  else
		return NFmiPoint(dx/len,dy/len);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Estimate initial tangent
	 *
	 * Note: The code assumes a closed polygon.
	 *
	 * \param thePath The path from which to estimate
	 * \return The tangent estimate
	 */
	// ----------------------------------------------------------------------

	NFmiPoint ComputeInitialTangent(const NFmiPathData & thePath)
	{
	  const int n = thePath.size();

	  const double dx1 = thePath[0].X() - thePath[n-2].X();
	  const double dy1 = thePath[0].Y() - thePath[n-2].Y();

	  const double dx2 = thePath[1].X() - thePath[0].X();
	  const double dy2 = thePath[1].Y() - thePath[0].Y();

	  const double dx = (dx1+dx2)/2;
	  const double dy = (dy1+dy2)/2;

	  const double len = sqrt(dx*dx+dy*dy);
	  if(len==0)
		return NFmiPoint(0,0);
	  else
		return NFmiPoint(dx/len,dy/len);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Utility subroutine for SimpleFit
	 *
	 * This generates a cubic Bezier fit recursively.
	 *
	 * \param thePath The path to approximate
	 * \param theFirst The first index (0 at the start)
	 * \param theLast The last index (path size -1 at the start)
	 * \param theTangent1 The left tangent
	 * \param theTangent2 The right tangent
	 * \param theSquaredMaxError The maximum allowed error squared
	 * \return The Bezier path
	 */
	// ----------------------------------------------------------------------

	NFmiPath RecursiveFit(const NFmiPathData & thePath,
											 int theFirst,
											 int theLast,
											 const NFmiPoint & theTangent1,
											 const NFmiPoint & theTangent2,
											 double theSquaredMaxError)
	{
	  NFmiPath outpath;
	  return outpath;
	}


	// ----------------------------------------------------------------------
	/*!
	 * \brief Generate tolerance bezier approximation for regular path segment
	 *
	 * A regular path segment starts with a moveto, is followed by
	 * several lineto commands and may or may not be closed.
	 *
	 * The algorithm is from Graphics Gems. The version by Philip J. Schneider
	 * was used as the basis for the C++ implementation.
	 *
	 * \param thePath The path to cardinalize
	 * \param theMaxError The maximum allowed error
	 * \return The approximated path
	 */
	// ----------------------------------------------------------------------

	NFmiPath SimpleFit(const NFmiPath & thePath,
										  double theMaxError)
	{
	  // safety against too small paths
	  if(thePath.Size()<3)
		return thePath;

	  const NFmiPathData & path = thePath.Elements();
	  const bool isclosed = NFmiBezierTools::IsClosed(thePath);
	  
	  // Estimate tangent vectors at endpoints of the curve
	  // Note that tangents face "inward", i.e., towards the
	  // interior of the set of sampled points

	  const double squared_max_error = theMaxError*theMaxError;

	  NFmiPoint tangent1, tangent2;

	  if(!isclosed)
		{
		  tangent1 = ComputeLeftTangent(path);
		  tangent2 = ComputeRightTangent(path);
		}
	  else
		{
		  tangent1 = ComputeInitialTangent(path);
		  tangent2 = NFmiPoint(-tangent1.X(),-tangent1.Y());
		}

	  return RecursiveFit(path,
						  0,
						  thePath.Size()-1,
						  tangent1,
						  tangent2,
						  squared_max_error);
	}

  } // namespace anonymous


  namespace NFmiApproximateBezierFit
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Calculate a Bezier approximation
	 *
	 * The given tolerance determines how closely the approximation
	 * must follow the original points. The larger the tolerance,
	 * the smoother the approximation is likely to be (with less
	 * control points in the path).
	 *
	 * \param thePath The path to approximate
	 * \param theMaxError The maximum Euclidian distance error
	 * \return The converted path
	 */
	// ----------------------------------------------------------------------

	NFmiPath Fit(const NFmiPath & thePath, double theMaxError)
	{
	  using namespace NFmiBezierTools;

	  NFmiPath outpath;
	  Segments segments = SplitSegments(thePath);
	  for(Segments::const_iterator it = segments.begin();
		  it != segments.end();
		  ++it)
		{
		  if(it->second)
			outpath.Add(SimpleFit(it->first,theMaxError));
		  else
			outpath.Add(it->first);
		}

	  return outpath;
	}

  } // namespace NFmiApproximateBezierFit

} // namespace Imagine

// ======================================================================
