// ======================================================================
/*!
 * \file
 * \brief Interface of namespace Imagine::NFmiCardinalBezierFit
 */
// ======================================================================

#ifndef IMAGINE_NFMICARDINALBEZIERFIT_H
#define IMAGINE_NFMICARDINALBEZIERFIT_H

#include "NFmiBezierTools.h"

namespace Imagine
{
class NFmiPath;

namespace NFmiCardinalBezierFit
{
const NFmiPath Fit(const NFmiPath& thePath, double theSmoothness);

typedef NFmiBezierTools::NFmiPaths NFmiPaths;
const NFmiPaths Fit(const NFmiPaths& thePaths, double theMaxError);

}  // namespace NFmiCardinalBezierFit
}  // namespace Imagine

#endif  // IMAGINE_NFMICARDINALBEZIERFIT_H

// ======================================================================
