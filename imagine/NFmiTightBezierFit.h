// ======================================================================
/*!
 * \file
 * \brief Interface of namespace Imagine::NFmiTightBezierFit
 */
// ======================================================================

#pragma once

#include "NFmiBezierTools.h"

namespace Imagine
{
class NFmiPath;

namespace NFmiTightBezierFit
{
NFmiPath Fit(const NFmiPath& thePath, double theMaxError);

using NFmiPaths = NFmiBezierTools::NFmiPaths;
NFmiPaths Fit(const NFmiPaths& thePaths, double theMaxError);

}  // namespace NFmiTightBezierFit
}  // namespace Imagine

// ======================================================================
