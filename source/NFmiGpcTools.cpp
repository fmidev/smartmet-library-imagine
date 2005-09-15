// ======================================================================
/*!
 * \file
 * \brief Implemention of a namespace to wrap GPC library
 */
// ======================================================================

#ifdef UNIX

#include "NFmiGpcTools.h"
#include "NFmiPath.h"

extern "C" {
#include "gpc.h"
}

#include <cassert>
#include <list>

using namespace std;

namespace Imagine
{
  namespace
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Helper function to add contours to GPC polygon
	 */
	// ----------------------------------------------------------------------

	void add_points_to_polygon(gpc_polygon * thePolygon,
							   const list<NFmiPoint> & thePoints)
	{
	  gpc_vertex_list * vlist = new gpc_vertex_list;
	  vlist->num_vertices = thePoints.size();
	  vlist->vertex = reinterpret_cast<gpc_vertex *>(calloc(thePoints.size(),sizeof(gpc_vertex)));
	  int i=0;
	  for(list<NFmiPoint>::const_iterator it = thePoints.begin();
		  it != thePoints.end();
		  ++it)
		{
		  vlist->vertex[i].x = it->X();
		  vlist->vertex[i].y = it->Y();
		  i++;
		}
	  gpc_add_contour(thePolygon,vlist,0);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Helper class to construct GPC polygons
	 */
	// ----------------------------------------------------------------------

	class GpcPolygon
	{
	public:
	  ~GpcPolygon();
	  GpcPolygon();
	  GpcPolygon(const NFmiPath & thePath);
	  gpc_polygon * get_ptr() const { return itsData; }
	  const NFmiPath path() const;
	private:
	  GpcPolygon(const GpcPolygon & thePolygon);
	  GpcPolygon & operator=(const GpcPolygon & thePolygon);

	  gpc_polygon * itsData;
	  
	}; // class GpcPolygon

	// ----------------------------------------------------------------------
	/*!
	 * \brief GpcPolygon destructor
	 */
	// ----------------------------------------------------------------------

	GpcPolygon::~GpcPolygon()
	{
	  gpc_free_polygon(itsData);
	}
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief GpcPolygon constructor
	 */
	// ----------------------------------------------------------------------

	GpcPolygon::GpcPolygon()
	  : itsData(new gpc_polygon)
	{
	  itsData->num_contours = 0;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief GpcPolygon copy constructor
	 */
	// ----------------------------------------------------------------------

	GpcPolygon::GpcPolygon(const NFmiPath & thePath)
	  : itsData(new gpc_polygon)
	{
	  itsData->num_contours = 0;

	  list<NFmiPoint> vertices;

	  for(NFmiPathData::const_iterator it = thePath.Elements().begin();
		  it != thePath.Elements().end();
		  ++it)
		{
		  switch(it->Oper())
			{
			case kFmiMoveTo:
			  {
				if(!vertices.empty())
				  {
					add_points_to_polygon(itsData,vertices);
					vertices.clear();
				  }
				vertices.push_back(NFmiPoint(it->X(),it->Y()));
				break;
			  }
			case kFmiLineTo:
			case kFmiGhostLineTo:
			  vertices.push_back(NFmiPoint(it->X(),it->Y()));
			  break;
			case kFmiConicTo:
			case kFmiCubicTo:
			  throw runtime_error("GpcTools: Cannot pass splines to GPC");
			}
		}

	  if(!vertices.empty())
		add_points_to_polygon(itsData,vertices);

	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Create NFmiPath from GpcPolygon
	 */
	// ----------------------------------------------------------------------

	const NFmiPath GpcPolygon::path() const
	{
	  assert(itsData != 0);
	  NFmiPath p;
	  NFmiPoint startpoint(0,0);
	  double x = 0;
	  double y = 0;
	  for(int i=0; i<itsData->num_contours; i++)
		{
		  gpc_vertex_list & contour = itsData->contour[i];
		  for(int j=0; j<contour.num_vertices; j++)
			{
			  x = contour.vertex[j].x;
			  y = contour.vertex[j].y;
			  if(j==0)
				{
				  if(!p.Empty())
					p.LineTo(startpoint.X(),startpoint.Y());
				  startpoint.Set(x,y);
				  p.MoveTo(x,y);
				}
			  else
				p.LineTo(x,y);
			}
		}
	  if(!p.Empty() && (startpoint.X() != x || startpoint.Y() != y))
		p.LineTo(startpoint.X(),startpoint.Y());

	  return p;
	}

  } // namespace anonymous

  namespace NFmiGpcTools
  {
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief Logical MINUS (difference)
	 */
	// ----------------------------------------------------------------------

	const NFmiPath Minus(const NFmiPath & theP1, const NFmiPath & theP2)
	{
	  GpcPolygon p1(theP1);
	  GpcPolygon p2(theP2);
	  GpcPolygon p3;
	  gpc_polygon_clip(GPC_DIFF,p1.get_ptr(),p2.get_ptr(),p3.get_ptr());
	  return p3.path();
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Logical AND (intersection)
	 */
	// ----------------------------------------------------------------------

	const NFmiPath And(const NFmiPath & theP1, const NFmiPath & theP2)
	{
	  GpcPolygon p1(theP1);
	  GpcPolygon p2(theP2);
	  GpcPolygon p3;
	  gpc_polygon_clip(GPC_INT,p1.get_ptr(),p2.get_ptr(),p3.get_ptr());
	  return p3.path();
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Logical OR (union)
	 */
	// ----------------------------------------------------------------------

	const NFmiPath Or(const NFmiPath & theP1, const NFmiPath & theP2)
	{
	  GpcPolygon p1(theP1);
	  GpcPolygon p2(theP2);
	  GpcPolygon p3;
	  gpc_polygon_clip(GPC_UNION,p1.get_ptr(),p2.get_ptr(),p3.get_ptr());
	  return p3.path();
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Logical XOR (exclusive-or)
	 */
	// ----------------------------------------------------------------------

	const NFmiPath Xor(const NFmiPath & theP1, const NFmiPath & theP2)
	{
	  GpcPolygon p1(theP1);
	  GpcPolygon p2(theP2);
	  GpcPolygon p3;
	  gpc_polygon_clip(GPC_XOR,p1.get_ptr(),p2.get_ptr(),p3.get_ptr());
	  return p3.path();
	}
	
  } // namespace NFmiGpcTools
} // namespace Imagine

#endif // UNIX

// ======================================================================
