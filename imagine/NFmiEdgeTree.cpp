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

#include "NFmiEdgeTree.h"
#include <macgyver/Exception.h>
#include <list>
#include <stdexcept>

using namespace std;

namespace Imagine
{
// ----------------------------------------------------------------------
// Add a contour edge to the set of unique edges. If the edge exists in
// the set already, it is removed.
// ----------------------------------------------------------------------

void NFmiEdgeTree::Add(const NFmiEdge& theEdge)
{
  try
  {
    if (itsLinesOnly && !theEdge.Exact())
      return;
    // If the edge is a 0-length line, ignore it

    if (theEdge.GetX1() != theEdge.GetX2() || theEdge.GetY1() != theEdge.GetY2())
    {
      if (theEdge.Fixed())
        itsMultiEdges.insert(theEdge);
      else
      {
        pair<EdgeTreeType::iterator, bool> result = itsEdges.insert(theEdge);

        // If insertion was unsuccessful, the element already existed,
        // and we wish to delete it. We no longer need to find the element,
        // it is pointed to by the earlier find iterator.

        if (!result.second)
          itsEdges.erase(result.first);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Add another edge tree into this one.
// ----------------------------------------------------------------------

void NFmiEdgeTree::Add(const NFmiEdgeTree& theTree)
{
  try
  {
    for (const auto& itsEdge : theTree.itsEdges)
      Add(itsEdge);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
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

NFmiPath NFmiEdgeTree::Path() const
{
  try
  {
    // The workhorse is a list of open paths. As soon as one
    // becomes closed, it is moved into outpath. Since outpath
    // is always closed, having the new path begin with a moveto
    // makes it simple to add the new (closed) path segment - we
    // just append the moves to the end.

    list<NFmiPath> paths;
    return Path(paths);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

namespace
{
// The open paths matching the end points of an edge
struct PathMatch
{
  bool fMatchedFirst = false;  // No matches for either endpoint yet
  bool fMatchedLast = false;

  bool fMatchedFirstToFirst = false;  // Matched beginning or end?
  bool fMatchedLastToFirst = false;

  list<NFmiPath>::iterator bestFirstIter;  // Best matching iterators
  list<NFmiPath>::iterator bestLastIter;
};

// Search the open path endpoints matching exactly either end point
// of the edge being added. We must match both end point to
// both end points of all open paths, that is N*4 cases
// where N is the (smallish) number of open paths.
PathMatch match_edge(list<NFmiPath>& paths, const NFmiEdge& edge)
{
  PathMatch m;
  for (auto pathiter = paths.begin(); pathiter != paths.end(); ++pathiter)
  {
    float firstx = pathiter->Elements().front().x;
    float firsty = pathiter->Elements().front().y;
    float lastx = pathiter->Elements().back().x;
    float lasty = pathiter->Elements().back().y;

    if (firstx == edge.GetX1() && firsty == edge.GetY1())
    {
      m.fMatchedFirst = true;
      m.fMatchedFirstToFirst = true;
      m.bestFirstIter = pathiter;
    }
    if (lastx == edge.GetX1() && lasty == edge.GetY1())
    {
      m.fMatchedFirst = true;
      m.fMatchedFirstToFirst = false;
      m.bestFirstIter = pathiter;
    }
    if (firstx == edge.GetX2() && firsty == edge.GetY2())
    {
      m.fMatchedLast = true;
      m.fMatchedLastToFirst = true;
      m.bestLastIter = pathiter;
    }
    if (lastx == edge.GetX2() && lasty == edge.GetY2())
    {
      m.fMatchedLast = true;
      m.fMatchedLastToFirst = false;
      m.bestLastIter = pathiter;
    }
  }
  return m;
}

// Append a line or a ghostline to the end of a path
void append_line(NFmiPath& path, float x, float y, bool exact)
{
  if (exact)
    path.LineTo(x, y);
  else
    path.GhostLineTo(x, y);
}

// Insert a line or a ghostline to the beginning of a path
void insert_line(NFmiPath& path, float x, float y, bool exact)
{
  if (exact)
    path.InsertLineTo(x, y);
  else
    path.InsertGhostLineTo(x, y);
}

// Connect an edge matching an open path at both ends
void connect_edge(list<NFmiPath>& paths, NFmiPath& outpath, const PathMatch& m, bool exact)
{
  // Close a path with the edge, add it to the final output path

  if (m.bestFirstIter == m.bestLastIter)
  {
    if (exact)
      m.bestFirstIter->CloseLineTo();
    else
      m.bestFirstIter->CloseGhostLineTo();
    outpath.Add(*m.bestFirstIter);
    paths.erase(m.bestFirstIter);
  }

  // Combine into one by appending firstpoint match
  // to lastpoint match

  else if (m.fMatchedFirstToFirst)
  {
    m.bestLastIter->Add(*m.bestFirstIter, exact);
    paths.erase(m.bestFirstIter);
  }
  else
  {
    m.bestFirstIter->Add(*m.bestLastIter, exact);
    paths.erase(m.bestLastIter);
  }
}

// Add an edge to the open paths, moving closed paths to the output path
void add_edge(
    list<NFmiPath>& paths, NFmiPath& outpath, const NFmiEdge& edge, const PathMatch& m, bool exact)
{
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

#define PathCase(a, b, c, d) ((((a) * 2 + (b)) * 2 + (c)) * 2 + (d))

  switch (PathCase(m.fMatchedFirst, m.fMatchedLast, m.fMatchedFirstToFirst, m.fMatchedLastToFirst))
  {
      // No match - we begin a new path

    case (PathCase(false, false, false, false)):
    {
      // Block required for tmp cleanup!
      NFmiPath tmp;
      paths.push_back(tmp);
      paths.back().MoveTo(edge.GetX1(), edge.GetY1());
      append_line(paths.back(), edge.GetX2(), edge.GetY2(), exact);
      break;
    }

      // Only first point matches, and it matches last point of path

    case (PathCase(true, false, false, false)):
      append_line(*m.bestFirstIter, edge.GetX2(), edge.GetY2(), exact);
      break;

      // Only first point matches, and it matches first point of path

    case (PathCase(true, false, true, false)):
      insert_line(*m.bestFirstIter, edge.GetX2(), edge.GetY2(), exact);
      break;

      // Only second point matches, and it matches last point of path

    case (PathCase(false, true, false, false)):
      append_line(*m.bestLastIter, edge.GetX1(), edge.GetY1(), exact);
      break;

      // Only second point matches, and it matches first point of path

    case (PathCase(false, true, false, true)):
      insert_line(*m.bestLastIter, edge.GetX1(), edge.GetY1(), exact);
      break;

      // Both points match, both match the endpoints or startpoints

    case (PathCase(true, true, false, false)):

      m.bestFirstIter->AddReverse(*m.bestLastIter, exact);
      paths.erase(m.bestLastIter);
      break;

    case (PathCase(true, true, true, true)):

      m.bestFirstIter->Insert(*m.bestLastIter, exact);
      paths.erase(m.bestLastIter);
      break;

      // Both points match, first endpoint, last firstpoint
      // In this case it is possible that the edge connects
      // the endpoints of some path. It is not possible
      // in the above cases, since in them the edge matches
      // either first points or last points.

    case (PathCase(true, true, false, true)):
    case (PathCase(true, true, true, false)):
      connect_edge(paths, outpath, m, exact);
      break;

    default:
      throw Fmi::Exception(BCP, "Internal error in NFmiContourTree::Path()");
  }
}
}  // namespace

NFmiPath NFmiEdgeTree::Path(list<NFmiPath>& paths) const
{
  try
  {
    // The result is a path containing only closed subpaths

    NFmiPath outpath;

    // Handle empty path

    if (itsEdges.empty() && itsMultiEdges.empty())
      return outpath;

    // Build from multiedges & edges

    MultiEdgeTreeType edges(itsMultiEdges);
    for (const auto& itsEdge : itsEdges)
    {
      edges.insert(itsEdge);
    }

    // An iterator for the open paths, we go through it again
    // for each edge in the tree. Note that there most likely
    // are only a couple open paths in the list, unless the
    // contours are extremely complex.

    list<NFmiPath>::iterator pathiter;

    // Iterate through the edges in the tree

    EdgeTreeType::const_iterator iter;

    for (iter = edges.begin(); iter != edges.end(); ++iter)
    {
      // The edge to be handled is assigned to a convenience variable
      // since avoiding *-dereferences makes code easier to follow.

      const NFmiEdge& edge = *iter;

      // Find out which still open paths have points closests
      // to either one of the end points of the edge.

      PathMatch match = match_edge(paths, edge);

      add_edge(paths, outpath, edge, match, edge.Exact() || itsConvertGhostLines);
    }

    // Append the non-closed segments

    for (pathiter = paths.begin(); pathiter != paths.end(); ++pathiter)
      outpath.Add(*pathiter);

    return outpath;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Add the contour tree to a fill map
// ----------------------------------------------------------------------

void NFmiEdgeTree::Add(NFmiFillMap& theMap) const
{
  try
  {
    // The iterator for traversing the data

    auto iter = Edges().begin();

    for (; iter != Edges().end(); ++iter)
      theMap.Add(iter->GetX1(), iter->GetY1(), iter->GetX2(), iter->GetY2());
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

}  // namespace Imagine

// ----------------------------------------------------------------------
