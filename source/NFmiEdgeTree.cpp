// ======================================================================
//
// A class to hold unique edges.
//
// History:
//
// 23.10.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================

#ifdef WIN32
#pragma warning(disable : 4786) // poistaa n kpl VC++ k��nt�j�n varoitusta
#endif

#include "NFmiEdgeTree.h"

using namespace std;

// ----------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------

NFmiEdgeTree::NFmiEdgeTree(void)
{
#ifdef EDGETREE_HASH
  itsEdges.resize(10000);
#endif
}

// ----------------------------------------------------------------------
// Add a contour edge to the set of unique edges. If the edge exists in
// the set already, it is removed.
// ----------------------------------------------------------------------

void NFmiEdgeTree::Add(const NFmiEdge& theEdge)
{
  
  // If the edge is a 0-length line, ignore it
  
  if(theEdge.X1() == theEdge.X2() && theEdge.Y1() == theEdge.Y2())
    return;
  
  // Insert operator returns an iterator, bool pair
  
  pair<EdgeTreeType::iterator, bool> result = itsEdges.insert(theEdge);
  
  
  // If insertion was unsuccessful, the element already existed,
  // and we wish to delete it. We no longer need to find the element,
  // it is pointed to by the earlier find iterator. Hence this is
  // an optimally fast log(N) operation for balanced trees.
  
  if(!result.second)
    itsEdges.erase(result.first);
  
}

// ----------------------------------------------------------------------
// Add another edge tree into this one.
// ----------------------------------------------------------------------

void NFmiEdgeTree::Add(const NFmiEdgeTree & theTree)
{
  for(EdgeTreeType::const_iterator iter = theTree.itsEdges.begin();
	  iter != theTree.itsEdges.end();
	  ++iter)
	Add(*iter);
}

// ----------------------------------------------------------------------
// Build a path from the tree.
//
// All path segments will be of maximum possible length. Hence, if
// the tree contains any closed polygons, they will be found. Any
// non-closed segment will not match the end points of any other
// non-closed segment.
//
// ----------------------------------------------------------------------

NFmiPath NFmiEdgeTree::Path(void) const
{
  // The result is a path containing only closed subpaths
  
  NFmiPath outpath;
  
  // Handle empty path
  
  if(itsEdges.empty()) return outpath;
  
  // The workhorse is a list of open paths. As soon as one
  // becomes closed, it is moved into outpath. Since outpath
  // is always closed, having the new path begin with a moveto
  // makes it simple to add the new (closed) path segment - we
  // just append the moves to the end.
  
  list<NFmiPath> paths;
  
  // The line segments of the tree are for convenience
  // assigned to a shorthand variable
  
  const EdgeTreeType edges(Edges());
  
  // An iterator for the open paths, we go through it again
  // for each edge in the tree. Note that there most likely
  // are only a couple open paths in the list, unless the
  // contours are extremely complex.
  
  list<NFmiPath>::iterator pathiter;
  
  list<NFmiPath>::iterator bestFirstIter;	// Best matching iterators
  list<NFmiPath>::iterator bestLastIter;
  
  // Iterate through the edges in the tree
  
  EdgeTreeType::const_iterator iter;
  
  for(iter=edges.begin() ; iter!=edges.end(); ++iter)
    {
      // The edge to be handled is assigned to a convenience variable
      // since avoiding *-dereferences makes code easier to follow.
	  
      const NFmiEdge & edge = *iter;
	  
      // Find out which still open paths have points closests
      // to either one of the end points of the edge.
      
      bool fMatchedFirst = false;	// No matches for either endpoint yet
      bool fMatchedLast  = false;
	  
      bool fMatchedFirstToFirst = false;	// Matched beginning or end?
      bool fMatchedLastToFirst = false;
	  
      // Search the open path endpoints matching exactly either end point
      // of the edge being added. We must match both end point to
      // both end points of all open paths, that is N*4 cases
      // where N is the (smallish) number of open paths.
	  
      for(pathiter=paths.begin(); pathiter!=paths.end(); ++pathiter)
		{
		  float firstx = pathiter->Elements().front().X();
		  float firsty = pathiter->Elements().front().Y();
		  float lastx  = pathiter->Elements().back().X();
		  float lasty  = pathiter->Elements().back().Y();
		  
		  if(firstx==edge.X1() && firsty==edge.Y1())
			{
			  fMatchedFirst = true;
			  fMatchedFirstToFirst = true;
			  bestFirstIter = pathiter;
			}
		  if(lastx==edge.X1() && lasty==edge.Y1())
			{
			  fMatchedFirst = true;
			  fMatchedFirstToFirst = false;
			  bestFirstIter = pathiter;
			}
		  if(firstx==edge.X2() && firsty==edge.Y2())
			{
			  fMatchedLast = true;
			  fMatchedLastToFirst = true;
			  bestLastIter = pathiter;
			}
		  if(lastx==edge.X2() && lasty==edge.Y2())
			{
			  fMatchedLast = true;
			  fMatchedLastToFirst = false;
			  bestLastIter = pathiter;
			}
		}
	  
      // We handle the cases separately:
      //
      // 1. Only first point matches
      // 2. Only second point matches
      // 3. Both points match
      // 4. No points match
      //
      // A help macro to handle the possible cases.
      // Note that the default values of the last two
      // variables is false, and cannot be anything but
      // false unless the respective variables a,b are true.
	  
#define PathCase(a,b,c,d) ((((a)*2+(b))*2+(c))*2+d)
	  
      switch(PathCase(fMatchedFirst,fMatchedLast,fMatchedFirstToFirst,fMatchedLastToFirst))
		{
		  // No match - we begin a new path
		  
		case(PathCase(false,false,false,false)):
		  {
			// Block required for tmp cleanup!
			NFmiPath tmp;
			paths.push_back(tmp);
			paths.back().MoveTo(edge.X1(),edge.Y1());
			if(edge.Exact())
			  paths.back().LineTo(edge.X2(),edge.Y2());
			else
			  paths.back().GhostLineTo(edge.X2(),edge.Y2());
			break;
		  }
		  
		  // Only first point matches, and it matches last point of path
		  
		case(PathCase(true,false,false,false)):
		  if(edge.Exact())
			bestFirstIter->LineTo(edge.X2(),edge.Y2());
		  else
			bestFirstIter->GhostLineTo(edge.X2(),edge.Y2());
		  break;
		  
		  // Only first point matches, and it matches first point of path
		  
		case(PathCase(true,false,true,false)):
		  if(edge.Exact())
			bestFirstIter->InsertLineTo(edge.X2(),edge.Y2());
		  else
			bestFirstIter->InsertGhostLineTo(edge.X2(),edge.Y2());
		  break;
		  
		  // Only second point matches, and it matches last point of path
		  
		case(PathCase(false,true,false,false)):
		  if(edge.Exact())
			bestLastIter->LineTo(edge.X1(),edge.Y1());
		  else
			bestLastIter->GhostLineTo(edge.X1(),edge.Y1());
		  break;
		  
		  // Only second point matches, and it matches first point of path
		  
		case(PathCase(false,true,false,true)):
		  if(edge.Exact())
			bestLastIter->InsertLineTo(edge.X1(),edge.Y1());
		  else
			bestLastIter->InsertGhostLineTo(edge.X1(),edge.Y1());
		  break;
		  
		  // Both points match, both match the endpoints or startpoints
		  
		case(PathCase(true,true,false,false)):
		  
		  bestFirstIter->AddReverse(*bestLastIter,edge.Exact());
		  paths.erase(bestLastIter);
		  break;
		  
		case(PathCase(true,true,true,true)):
		  
		  bestFirstIter->Insert(*bestLastIter,edge.Exact());
		  paths.erase(bestLastIter);
		  break;
		  
		  // Both points match, first endpoint, last firstpoint
		  // In this case it is possible that the edge connects
		  // the endpoints of some path. It is not possible
		  // in the above cases, since in them the edge matches
		  // either first points or last points.
		  
		case(PathCase(true,true,false,true)):
		case(PathCase(true,true,true,false)):
		  
		  // Close a path with the edge, add it to the final output path
		  
		  if( bestFirstIter == bestLastIter )
			{
			  if(edge.Exact())
				bestFirstIter->CloseLineTo();
			  else
				bestFirstIter->CloseGhostLineTo();
			  outpath.Add(*bestFirstIter);
			  paths.erase(bestFirstIter);
			}
		  
		  // Combine into one by appending firstpoint match
		  // to lastpoint match
		  
		  else if(fMatchedFirstToFirst)
			{
			  bestLastIter->Add(*bestFirstIter,edge.Exact());
			  paths.erase(bestFirstIter);
			}
		  else
			{
			  bestFirstIter->Add(*bestLastIter,edge.Exact());
			  paths.erase(bestLastIter);
			}
		  break;
		  
		default:
		  cerr << "Internal error in NFmiContourTree::Path()\n";
		  exit(1);
		}
	  
    }
  
  // Append the non-closed segments
  
  for(pathiter=paths.begin(); pathiter!=paths.end(); ++pathiter)
    outpath.Add(*pathiter);
  
  return outpath;
  
}

// ----------------------------------------------------------------------
// Add the contour tree to a fill map
// ----------------------------------------------------------------------

void NFmiEdgeTree::Add(NFmiFillMap & theMap) const
{
  // The iterator for traversing the data
  
  EdgeTreeType::const_iterator iter = Edges().begin();
  
  for( ; iter!=Edges().end(); ++iter)
    theMap.Add(iter->X1(),iter->Y1(),iter->X2(),iter->Y2());  
}

// ----------------------------------------------------------------------
