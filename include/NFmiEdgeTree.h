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

#ifndef _NFMIEDGETREE_H
#define _NFMIEDGETREE_H

// Do not use hash_set, use regular set

#undef EDGETREE_HASH

// Essential includes:

#include <utility>	// pairs
#include <set>		// sets
#include <vector>	// vectors
#include <algorithm>	// insert
#ifdef EDGETREE_HASH
#include <hash_set>
#endif

#include "NFmiEdge.h"		// Input edges
#include "NFmiPath.h"		// Path generation
#include "NFmiFillMap.h"	// Fill map generation and rendering

// ----------------------------------------------------------------------
// A working class, holding a tree of unique edges
// ----------------------------------------------------------------------

class NFmiEdgeTree : public NFmiDrawable
{
public:

#ifdef  EDGETREE_HASH
  typedef std::hash_set<NFmiEdge,NFmiEdgeHasher> EdgeTreeType;
#else
  typedef std::set<NFmiEdge> EdgeTreeType;
#endif

  // Constructors

  NFmiEdgeTree(void);

  // Destructors

  virtual ~NFmiEdgeTree(void) {}

  // Access to edge-data for iterating through the data, not for
  // modifying it.

  const EdgeTreeType & Edges(void) const { return itsEdges; };

  // Adding a single edge

  void Add(const NFmiEdge & theEdge);

  // Build a path from the tree.

  NFmiPath Path(void) const;

  // Add the tree to a fill map

  void Add(NFmiFillMap & theMap) const;

private:

  EdgeTreeType itsEdges;
};

// ----------------------------------------------------------------------

#endif	// _NFMIEDGETREE_H

