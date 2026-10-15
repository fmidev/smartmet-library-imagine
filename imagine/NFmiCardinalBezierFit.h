// ======================================================================
/*!
 * \file
 * \brief Interface of namespace Imagine::NFmiCardinalBezierFit
 */
// ======================================================================

#pragma once

#include "NFmiBezierTools.h"

namespace Imagine
{
class NFmiPath;

namespace NFmiCardinalBezierFit
{
NFmiPath Fit(const NFmiPath& thePath, double theSmoothness);

using NFmiPaths = NFmiBezierTools::NFmiPaths;
NFmiPaths Fit(const NFmiPaths& thePaths, double theMaxError);

}  // namespace NFmiCardinalBezierFit
}  // namespace Imagine

// ======================================================================
