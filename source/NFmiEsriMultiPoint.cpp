// ======================================================================
//
// Esri Shapefile Techinical Description, page 6
//
//
// Position	Field	Value	Type	Number	Endian
//
// Byte 0	Type	8	int	1	little
// Byte 4	Box	Box	double	4	little
// Byte 36	NPoint	NPoints	int	1	little
// Byte 40	Points	Points	Point	NPoints	little
//
// ======================================================================

#include "NFmiEsriMultiPoint.h"

using namespace NFmiEsriBuffer;	// Conversion tools
using namespace std;

// ----------------------------------------------------------------------
// Constructor based on a character buffer
// ----------------------------------------------------------------------

NFmiEsriMultiPoint::NFmiEsriMultiPoint(const string & theBuffer, int thePos, int theNumber)
  : NFmiEsriElement(kFmiEsriMultiPoint, theNumber)
{
  int npoints = LittleEndianInt(theBuffer,thePos+36);
  
  // Speed up by reserving enough space already
  
  itsPoints.reserve(itsPoints.size()+npoints);
  
  for(int i=0; i<npoints; i++)
    {
      // Start position 40, then 2 doubles (16) for each point
      int pointpos = thePos+40+i*16;
      Add(NFmiEsriPoint(LittleEndianDouble(theBuffer,pointpos),
						LittleEndianDouble(theBuffer,pointpos+8)));
    }
}

// ----------------------------------------------------------------------
// Calculating string buffer size
// ----------------------------------------------------------------------

int NFmiEsriMultiPoint::StringSize(void) const
{
  return (4			// the type	: 1 int
		  +4*8			// bounding box : 4 doubles
		  +4			// numpoints	: 1 int
		  +NumPoints()*2*8	// points	: 2n doubles
		  );
}

// ----------------------------------------------------------------------
// Writing element
// ----------------------------------------------------------------------

void NFmiEsriMultiPoint::Write(ostream & os) const
{
  os << LittleEndianInt(Type())
     << LittleEndianDouble(Box().Xmin())
     << LittleEndianDouble(Box().Ymin())
     << LittleEndianDouble(Box().Xmax())
     << LittleEndianDouble(Box().Ymax())
     << LittleEndianInt(NumPoints());
  
  for(int i=0; i<NumPoints(); i++)
    {
      os << LittleEndianDouble(Points()[i].X())
		 << LittleEndianDouble(Points()[i].Y());
    }
}

// ----------------------------------------------------------------------
// Projecting the data
// ----------------------------------------------------------------------

void NFmiEsriMultiPoint::Project(const NFmiEsriProjector & theProjector)
{
  itsBox.Init();
  for(int i=0; i<NumPoints(); i++)
    {
      NFmiEsriPoint tmp(itsPoints[i].X(),itsPoints[i].Y());
      tmp = theProjector(tmp);
      itsPoints[i].X(tmp.X());
      itsPoints[i].Y(tmp.Y());
      itsBox.Update(tmp.X(),tmp.Y());
    }
}

// ======================================================================
