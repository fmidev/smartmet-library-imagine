// ======================================================================
/*!
 * \file
 * \brief Interface of namespace Imagine::NFmiBezierTools
 */
// ======================================================================

// Note: This namespace contains tools which are mostly useful
// only to the various Bezier fitting namespaces.

#ifndef IMAGINE_NFMIBEZIERTOOLS_H
#define IMAGINE_NFMIBEZIERTOOLS_H

#include "NFmiDef.h"

#include <list>
#include <vector>

class NFmiPoint;

namespace Imagine
{
  template<class T> class NFmiCounter;
  class NFmiPath;

  namespace NFmiBezierTools
  {

	typedef std::list<std::pair<NFmiPath,bool> > Segments;
	typedef std::vector<NFmiPath> NFmiPaths;
	typedef std::list<NFmiPath> PathList;

	bool IsClosed(const NFmiPath & thePath);

	NFmiCounter<NFmiPoint> VertexCounts(const NFmiPaths & thePaths);

	Segments SplitSegments(const NFmiPath & thePath);

	PathList SplitPath(const NFmiPath & thePath);

	PathList SplitPath(const NFmiPath & thePath,
					   const NFmiCounter<NFmiPoint> & theCounts);

	double BezierLength(const NFmiPath & thePath, double theRelativeAccuracy);

  } // namespace NFmiBezierTools
} // namespace Imagine

#endif // IMAGINE_NFMIBEZIERTOOLS_H

// ======================================================================
