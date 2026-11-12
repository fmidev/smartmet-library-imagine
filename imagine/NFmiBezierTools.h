// ======================================================================
/*!
 * \file
 * \brief Interface of namespace Imagine::NFmiBezierTools
 */
// ======================================================================

// Note: This namespace contains tools which are mostly useful
// only to the various Bezier fitting namespaces.

#pragma once

#include <newbase/NFmiPoint.h>
#include <iterator>
#include <list>
#include <utility>
#include <vector>

namespace Imagine
{
class NFmiPath;
template <class T>
class NFmiCounter;

namespace NFmiBezierTools
{
using Segments = std::list<std::pair<NFmiPath, bool> >;
using NFmiPaths = std::vector<NFmiPath>;
using PathList = std::list<NFmiPath>;

bool IsClosed(const NFmiPath& thePath);

NFmiCounter<NFmiPoint> VertexCounts(const NFmiPaths& thePaths);

Segments SplitSegments(const NFmiPath& thePath);

PathList SplitPath(const NFmiPath& thePath);

PathList SplitPath(const NFmiPath& thePath, const NFmiCounter<NFmiPoint>& theCounts);

double BezierLength(const NFmiPath& thePath, double theRelativeAccuracy);

}  // namespace NFmiBezierTools
}  // namespace Imagine

// ======================================================================
