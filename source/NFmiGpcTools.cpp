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

namespace Imagine
{
  namespace
  {
	// ----------------------------------------------------------------------
	/*!
	 * \brief Helper class to construct GPC polygons
	 */
	// ----------------------------------------------------------------------

	class GpcPolygon
	{
	public:
	  ~GpcPolygon() { gpc_free_polygon(itsData); }
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
	 * \brief GpcPolygon constructor
	 */
	// ----------------------------------------------------------------------

	GpcPolygon::GpcPolygon()
	  : itsData(new gpc_polygon)
	{
	  itsData->num_contours = 0;
	  itsData->hole = 0;
	  itsData->contour = 0;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief GpcPolygon copy constructor
	 */
	// ----------------------------------------------------------------------

	GpcPolygon::GpcPolygon(const NFmiPath & thePath)
	  : itsData(new gpc_polygon)
	{
	  // ...
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Create NFmiPath from GpcPolygon
	 */
	// ----------------------------------------------------------------------

	const NFmiPath GpcPolygon::path() const
	{
	  NFmiPath p;
	  // ...
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
