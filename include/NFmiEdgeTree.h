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

#ifndef IMAGINE_NFMIEDGETREE_H
#define IMAGINE_NFMIEDGETREE_H

// Essential includes:

#include "NFmiEdge.h"		// Input edges
#include "NFmiPath.h"		// Path generation
#include "NFmiFillMap.h"	// Fill map generation and rendering

#include <utility>	// pairs
#include <set>		// sets
#include <vector>	// vectors
#include <algorithm>	// insert

namespace Imagine
{

  // ----------------------------------------------------------------------
  // A working class, holding a tree of unique edges
  // ----------------------------------------------------------------------
  
  class _FMI_DLL NFmiEdgeTree : public NFmiDrawable
  {
  public:
	
	// Constructors
	
	NFmiEdgeTree(void) : itsLinesOnly(false), itsEdges() { }
	
	// Destructors
	
	virtual ~NFmiEdgeTree(void) {}
	
	// Adding a single edge
	
	void Add(const NFmiEdge & theEdge);
	
	// Adding another edge tree
	void Add(const NFmiEdgeTree & theTree);
	
	// Build a path from the tree.
	
	const NFmiPath Path(void) const;
	
	// Add the tree to a fill map
	
	void Add(NFmiFillMap & theMap) const;

	void LinesOnly(bool theFlag){ itsLinesOnly = theFlag;}
	
  protected:

	typedef std::set<NFmiEdge> EdgeTreeType;
	
	// Access to edge-data for iterating through the data, not for
	// modifying it.
	
	const EdgeTreeType & Edges(void) const { return itsEdges; };

	bool itsLinesOnly;
	EdgeTreeType itsEdges;

  };

} // namespace Imagine
  
// ----------------------------------------------------------------------
  
#endif	// IMAGINE_NFMIEDGETREE_H
