// ======================================================================
//
// Esri Shapefile Techinical Description, page 7
//
//
// Position	Field	Value	Type	Number	Endian
//
// Byte 0	Type	3	int	1	little
// Byte 4	Box	Box	double	4	little
// Byte 36	NParts	NParts	int	1	little
// Byte 40	NPoints	NPoints	int	1	little
// Byte 44	Parts	Parts	int	NParts	little
// Byte X	Points	Points	point	NPoints	little
//
// Note: X = 44 + 4 * NumParts
//
// ======================================================================
#if (defined _MSC_VER) && (_MSC_VER < 1310) // uusi MSVC 7.1 (1310 eli ver 13.1 eli MSVC++ 7.1) hoitaa homman ilman pragmoja
#pragma warning(disable : 4786) // poistaa n kpl VC++ kääntäjän varoitusta (liian pitkä nimi >255 merkkiä joka johtuu 'puretuista' STL-template nimistä)
#endif

#include "NFmiEsriPolyLine.h"

using namespace Imagine::NFmiEsriBuffer;	// Conversion tools
using namespace std;

namespace Imagine
{

  // ----------------------------------------------------------------------
  // Constructor based on a character buffer
  // ----------------------------------------------------------------------
  
  NFmiEsriPolyLine::NFmiEsriPolyLine(const string & theBuffer, int thePos, int theNumber)
	: NFmiEsriElement(kFmiEsriPolyLine, theNumber)
	, itsBox()
	, itsParts()
	, itsPoints()
  {
	
	int nparts = LittleEndianInt(theBuffer,thePos+36);
	int npoints = LittleEndianInt(theBuffer,thePos+40);
	
	// Speed up by reserving enough space already
	// Sample roads file speedup was 1s!
	
	itsParts.reserve(itsParts.size()+nparts);
	itsPoints.reserve(itsPoints.size()+npoints);
	
	// Establish the parts
	
	int i=0;
	for(i=0; i<nparts; i++)
	  itsParts.push_back(LittleEndianInt(theBuffer,thePos+44+4*i));
	
	// And the points
	
	for(i=0; i<npoints; i++)
	  {
		int pointpos = thePos + 44 + 4 * nparts + 16*i;
		Add(NFmiEsriPoint(LittleEndianDouble(theBuffer,pointpos),
						  LittleEndianDouble(theBuffer,pointpos+8)));
	  }
  }
  
  // ----------------------------------------------------------------------
  // Calculating string buffer size
  // ----------------------------------------------------------------------
  
  int NFmiEsriPolyLine::StringSize(void) const
  {
	return (4			// the type	: 1 int
			+4*8			// bounding box : 4 doubles
			+4			// numparts	: 1 int
			+4			// numpoints	: 1 int
			+NumParts()*4		// parts	: np ints
			+NumPoints()*2*8	// points	: 2n doubles
			);
  }
  
  // ----------------------------------------------------------------------
  // Writing the element
  // ----------------------------------------------------------------------
  
  std::ostream & NFmiEsriPolyLine::Write(ostream & os) const
  {
	os << LittleEndianInt(Type())
	   << LittleEndianDouble(Box().Xmin())
	   << LittleEndianDouble(Box().Ymin())
	   << LittleEndianDouble(Box().Xmax())
	   << LittleEndianDouble(Box().Ymax())
	   << LittleEndianInt(NumParts())
	   << LittleEndianInt(NumPoints());
	
	int i=0;
	for(i=0; i<NumParts(); i++)
	  os << LittleEndianInt(Parts()[i]);
	
	for(i=0; i<NumPoints(); i++)
	  {
		os << LittleEndianDouble(Points()[i].X())
		   << LittleEndianDouble(Points()[i].Y());
	  }
	return os;
  }
  
  // ----------------------------------------------------------------------
  // Projecting the data
  // ----------------------------------------------------------------------
  
  void NFmiEsriPolyLine::Project(const NFmiEsriProjector & theProjector)
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

} // namespace Imagine
  
// ======================================================================
  
