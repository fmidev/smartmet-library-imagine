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

namespace Imagine
{
  class NFmiPath;

  namespace NFmiBezierTools
  {
	
	bool IsClosed(const NFmiPath & thePath);

	typedef std::list<std::pair<NFmiPath,bool> > Segments;
	Segments SplitSegments(const NFmiPath & thePath);

  } // namespace NFmiBezierTools
} // namespace Imagine

#endif // IMAGINE_NFMIBEZIERTOOLS_H

// ======================================================================
