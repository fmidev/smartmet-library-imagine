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

#include "NFmiFillMap.h"  // Fill map generation and rendering

#include <list>
#include <set>

namespace Imagine
{
// ----------------------------------------------------------------------
// A working class, holding a tree of unique edges
// ----------------------------------------------------------------------

class NFmiEdgeTree
    : public NFmiDrawable
{
 public:
  // Constructors

  NFmiEdgeTree() = default;
  // Destructors

  ~NFmiEdgeTree() override = default;
  // Adding a single edge

  void Add(const NFmiEdge& theEdge);

  // Adding another edge tree
  void Add(const NFmiEdgeTree& theTree);

  // Build a path from the tree.
  NFmiPath Path() const;

// Add the tree to a fill map
  void Add(NFmiFillMap& theMap) const override;

  void LinesOnly(bool theFlag) { itsLinesOnly = theFlag; }
  void ConvertGhostLines(bool theFlag) { itsConvertGhostLines = theFlag; }

 protected:
  NFmiPath Path(std::list<NFmiPath>& thePaths) const;

  using EdgeTreeType = std::set<NFmiEdge>;
  using MultiEdgeTreeType = std::multiset<NFmiEdge>;

  // Access to edge-data for iterating through the data, not for
  // modifying it.

  const EdgeTreeType& Edges() const { return itsEdges; }
  bool itsLinesOnly = false;
  bool itsConvertGhostLines = false;
  EdgeTreeType itsEdges;
  MultiEdgeTreeType itsMultiEdges;
};

}  // namespace Imagine

// ----------------------------------------------------------------------
