// ======================================================================
/*!
 * \file
 * \brief Interface of namespace Imagine::NFmiApproximateBezierFit
 */
// ======================================================================

#ifndef IMAGINE_NFMIAPPROXIMATEBEZIERFIT_H
#define IMAGINE_NFMIAPPROXIMATEBEZIERFIT_H

#include <vector>

namespace Imagine
{
  class NFmiPath;

  typedef std::vector<NFmiPath> NFmiPaths;

  namespace NFmiApproximateBezierFit
  {

	NFmiPath Fit(const NFmiPath & thePath, double theMaxError);

	NFmiPaths Fit(const NFmiPaths & thePaths, double theMaxError);

  } // namespace NFmiApproximateBezierFit
} // namespace Imagine

#endif // IMAGINE_NFMIAPPROXIMATEBEZIERFIT_H

// ======================================================================
