// ======================================================================
/*!
 * \file
 * \brief Interface of namespace Imagine::NFmiBezierTools
 */
// ======================================================================

// Note: This namespace contains tools which are mostly useful
// only to the various Bezier fitting namespaces.

#pragma once

#include <list>
#include <vector>

class NFmiPoint;

namespace Imagine
{
template <class T>
class NFmiCounter;
class NFmiPath;

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
