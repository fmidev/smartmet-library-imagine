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

// Essential includes:

#include <utility>	// pairs
#include <set>		// sets
#include <vector>	// vectors
#include <algorithm>	// insert

#include "NFmiEdge.h"		// Input edges
#include "NFmiPath.h"		// Path generation
#include "NFmiFillMap.h"	// Fill map generation and rendering

// ----------------------------------------------------------------------
// A working class, holding a tree of unique edges
// ----------------------------------------------------------------------

class _FMI_DLL NFmiEdgeTree : public NFmiDrawable
{
public:
  
  typedef std::set<NFmiEdge> EdgeTreeType;
  
  // Constructors
  
  NFmiEdgeTree(void) { }
  
  // Destructors
  
  virtual ~NFmiEdgeTree(void) {}
  
  // Access to edge-data for iterating through the data, not for
  // modifying it.
  
  const EdgeTreeType & Edges(void) const { return itsEdges; };
  
  // Adding a single edge
  
  void Add(const NFmiEdge & theEdge);

  // Adding another edge tree
  void Add(const NFmiEdgeTree & theTree);
  
  // Build a path from the tree.
  
  NFmiPath Path(void) const;
  
  // Add the tree to a fill map
  
  void Add(NFmiFillMap & theMap) const;
  
private:
  
  EdgeTreeType itsEdges;
};

// ----------------------------------------------------------------------

#endif	// _NFMIEDGETREE_H

