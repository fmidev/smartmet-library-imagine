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
#if (defined _MSC_VER) && (_MSC_VER < 1310) // uusi MSVC 7.1 (1310 eli ver 13.1 eli MSVC++ 7.1) hoitaa homman ilman pragmoja
#pragma warning(disable : 4786) // poistaa n kpl VC++ kääntäjän varoitusta (liian pitkä nimi >255 merkkiä joka johtuu 'puretuista' STL-template nimistä)
#endif

#include "NFmiEsriPoint.h"

using namespace Imagine::NFmiEsriBuffer;	// Conversion tools
using namespace std;

namespace Imagine
{

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
  
  std::ostream & NFmiEsriPoint::Write(ostream & os) const
  {
	os << LittleEndianInt(Type())
	   << LittleEndianDouble(X())
	   << LittleEndianDouble(Y());
	return os;
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

} // namespace Imagine
  
// ======================================================================
