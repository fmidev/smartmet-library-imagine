// ======================================================================
/*!
 * \file
 * \brief Interface of namespace Imagine::NFmiApproximateBezierFit
 */
// ======================================================================

#ifndef IMAGINE_NFMIAPPROXIMATEBEZIERFIT_H
#define IMAGINE_NFMIAPPROXIMATEBEZIERFIT_H

#include "NFmiBezierTools.h"

namespace Imagine
{
class NFmiPath;

namespace NFmiApproximateBezierFit
{
const NFmiPath Fit(const NFmiPath& thePath, double theMaxError);

typedef NFmiBezierTools::NFmiPaths NFmiPaths;
const NFmiPaths Fit(const NFmiPaths& thePaths, double theMaxError);

}  // namespace NFmiApproximateBezierFit
}  // namespace Imagine

#endif  // IMAGINE_NFMIAPPROXIMATEBEZIERFIT_H

// ======================================================================
