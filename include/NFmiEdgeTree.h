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

#include "imagine-config.h"

// Essential includes:

#include "NFmiEdge.h"		// Input edges
#include "NFmiPath.h"		// Path generation

#ifndef IMAGINE_WITH_CAIRO
# include "NFmiFillMap.h"	// Fill map generation and rendering
#endif

#include <utility>	// pairs
#include <set>		// sets
#include <vector>	// vectors
#include <algorithm>	// insert

namespace Imagine
{

  // ----------------------------------------------------------------------
  // A working class, holding a tree of unique edges
  // ----------------------------------------------------------------------
  
  class _FMI_DLL NFmiEdgeTree
#ifndef IMAGINE_WITH_CAIRO
    : public NFmiDrawable
#endif
  {
  public:
	
	// Constructors
	
	NFmiEdgeTree(void)
	  : itsLinesOnly(false)
	  , itsConvertGhostLines(false)
	  , itsEdges()
	{ }
	
	// Destructors
	
	virtual ~NFmiEdgeTree(void) {}
	
	// Adding a single edge
	
	void Add(const NFmiEdge & theEdge);
	
	// Adding another edge tree
	void Add(const NFmiEdgeTree & theTree);
	
	// Build a path from the tree.
	
	const NFmiPath Path(void) const;
	
	// Add the tree to a fill map
#ifndef IMAGINE_WITH_CAIRO
	void Add(NFmiFillMap & theMap) const;
#endif

	void LinesOnly(bool theFlag){ itsLinesOnly = theFlag;}
	void ConvertGhostLines(bool theFlag) { itsConvertGhostLines = theFlag; }
	
  protected:

	typedef std::set<NFmiEdge> EdgeTreeType;
	
	// Access to edge-data for iterating through the data, not for
	// modifying it.
	
	const EdgeTreeType & Edges(void) const { return itsEdges; };

	bool itsLinesOnly;
	bool itsConvertGhostLines;
	EdgeTreeType itsEdges;

  };

} // namespace Imagine
  
// ----------------------------------------------------------------------
  
#endif	// IMAGINE_NFMIEDGETREE_H
