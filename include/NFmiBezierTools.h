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

#include <list>
#include <vector>

class NFmiPoint;

namespace Imagine
{
  template<class T> class NFmiCounter;
  class NFmiPath;

  namespace NFmiBezierTools
  {

	bool IsClosed(const NFmiPath & thePath);

	typedef std::vector<NFmiPath> NFmiPaths;
	NFmiCounter<NFmiPoint> VertexCounts(const NFmiPaths & thePaths);

	typedef std::list<std::pair<NFmiPath,bool> > Segments;
	Segments SplitSegments(const NFmiPath & thePath);

	typedef std::list<NFmiPath> PathList;
	PathList SplitPath(const NFmiPath & thePath,
					   const NFmiCounter<NFmiPoint> & theCounts);

  } // namespace NFmiBezierTools
} // namespace Imagine

#endif // IMAGINE_NFMIBEZIERTOOLS_H

// ======================================================================
