// ======================================================================
//
// Esri Shapefile Techinical Description, page 6
//
//
// Position	Field	Value	Type	Number	Endian
//
// Byte 0	Type	1	int	1	little
// Byte 4	X	X	double	1	little
// Byte 12	Y	Y	double	1	little
//
// ======================================================================

#include "NFmiEsriPoint.h"

using namespace NFmiEsriBuffer;	// Conversion tools
using namespace std;

// ----------------------------------------------------------------------
// Constructor based on a character buffer
// ----------------------------------------------------------------------

NFmiEsriPoint::NFmiEsriPoint(const string & theBuffer, int thePos, int theNumber)
  : NFmiEsriElement(kFmiEsriPoint,theNumber)
  , itsX(LittleEndianDouble(theBuffer,thePos+4))
  , itsY(LittleEndianDouble(theBuffer,thePos+12))
{
}

// ----------------------------------------------------------------------
// Calculating string buffer size
// ----------------------------------------------------------------------

int NFmiEsriPoint::StringSize(void) const
{
  return (4+2*8);	// int + 2 doubles
}

// ----------------------------------------------------------------------
// Writing element
// ----------------------------------------------------------------------

void NFmiEsriPoint::Write(ostream & os) const
{
  os << LittleEndianInt(Type())
     << LittleEndianDouble(X())
     << LittleEndianDouble(Y());
}

// ----------------------------------------------------------------------
// Projecting the data
// ----------------------------------------------------------------------

void NFmiEsriPoint::Project(const NFmiEsriProjector & theProjector)
{
  NFmiEsriPoint tmp(X(),Y());
  tmp = theProjector(tmp);
  X(tmp.X());
  Y(tmp.Y());
}

// ======================================================================
