// ======================================================================
//
// See documentation in NFmiEdgeTree.cpp
//
// History:
//
// 23.10.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================

#pragma once

// Essential includes:

#include "NFmiEdge.h"  // Input edges
#include "NFmiPath.h"  // Path generation

#ifndef IMAGINE_WITH_CAIRO
#include "NFmiFillMap.h"  // Fill map generation and rendering
#endif

#include <list>
#include <set>

namespace Imagine
{
// ----------------------------------------------------------------------
// A working class, holding a tree of unique edges
// ----------------------------------------------------------------------

class NFmiEdgeTree
#ifndef IMAGINE_WITH_CAIRO
    : public NFmiDrawable
#endif
{
 public:
  // Constructors

  NFmiEdgeTree() : itsLinesOnly(false), itsConvertGhostLines(false), itsEdges() {}
  // Destructors

  ~NFmiEdgeTree() override {}
  // Adding a single edge

  void Add(const NFmiEdge& theEdge);

  // Adding another edge tree
  void Add(const NFmiEdgeTree& theTree);

  // Build a path from the tree.
  NFmiPath Path() const;

// Add the tree to a fill map
#ifndef IMAGINE_WITH_CAIRO
  void Add(NFmiFillMap& theMap) const override;
#endif

  void LinesOnly(bool theFlag) { itsLinesOnly = theFlag; }
  void ConvertGhostLines(bool theFlag) { itsConvertGhostLines = theFlag; }

 protected:
  NFmiPath Path(std::list<NFmiPath>& thePaths) const;

  using EdgeTreeType = std::set<NFmiEdge>;
  using MultiEdgeTreeType = std::multiset<NFmiEdge>;

  // Access to edge-data for iterating through the data, not for
  // modifying it.

  const EdgeTreeType& Edges() const { return itsEdges; };
  bool itsLinesOnly;
  bool itsConvertGhostLines;
  EdgeTreeType itsEdges;
  MultiEdgeTreeType itsMultiEdges;
};

}  // namespace Imagine

// ----------------------------------------------------------------------
