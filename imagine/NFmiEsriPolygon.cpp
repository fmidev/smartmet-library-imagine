// ======================================================================
//
// Esri Shapefile Techinical Description, page 8
//
//
// Position	Field	Value	Type	Number	Endian
//
// Byte 0	Type	5	int	1	little
// Byte 4	Box	Box	double	4	little
// Byte 36	NParts	NParts	int	1	little
// Byte 40	NPoints	NPoints	int	1	little
// Byte 44	Parts	Parts	int	NParts	little
// Byte X	Points	Points	point	NPoints	little
//
// ======================================================================

#include "NFmiEsriPolygon.h"
#include "NFmiEsriBuffer.h"
#include <macgyver/Exception.h>
#include <iostream>

using namespace Imagine::NFmiEsriBuffer;  // Conversion tools
using namespace std;

namespace Imagine
{
// ----------------------------------------------------------------------
// Copy constructor
// ----------------------------------------------------------------------

NFmiEsriPolygon::NFmiEsriPolygon(const NFmiEsriPolygon& thePolygon)
    : NFmiEsriElement(thePolygon),
      itsBox(thePolygon.itsBox),
      itsParts(thePolygon.itsParts),
      itsPoints(thePolygon.itsPoints)
{
}

// ----------------------------------------------------------------------
// Assignment operator
// ----------------------------------------------------------------------

NFmiEsriPolygon& NFmiEsriPolygon::operator=(const NFmiEsriPolygon& thePolygon)
{
  try
  {
    if (this != &thePolygon)
    {
      NFmiEsriElement::operator=(thePolygon);
      itsBox = thePolygon.itsBox;
      itsParts = thePolygon.itsParts;
      itsPoints = thePolygon.itsPoints;
    }
    return *this;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Cloning
// ----------------------------------------------------------------------

NFmiEsriElement* NFmiEsriPolygon::Clone() const
{
  try
  {
    return new NFmiEsriPolygon(*this);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Constructor based on a character buffer
// ----------------------------------------------------------------------

NFmiEsriPolygon::NFmiEsriPolygon(const string& theBuffer, int thePos, int theNumber)
    : NFmiEsriElement(kFmiEsriPolygon, theNumber), itsBox(), itsParts(), itsPoints()
{
  try
  {
    int nparts = LittleEndianInt(theBuffer, thePos + 36);
    int npoints = LittleEndianInt(theBuffer, thePos + 40);

    // Speed up by reserving enough space already

    itsParts.reserve(itsParts.size() + nparts);
    itsPoints.reserve(itsPoints.size() + npoints);

    // Establish the parts

    int i = 0;
    for (i = 0; i < nparts; i++)
      itsParts.push_back(LittleEndianInt(theBuffer, thePos + 44 + 4 * i));

    // And the points

    for (i = 0; i < npoints; i++)
    {
      int pointpos = thePos + 44 + 4 * nparts + 16 * i;
      Add(NFmiEsriPoint(LittleEndianDouble(theBuffer, pointpos),
                        LittleEndianDouble(theBuffer, pointpos + 8)));
    }
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Calculating string buffer size
// ----------------------------------------------------------------------

int NFmiEsriPolygon::StringSize(void) const
{
  try
  {
    return (4                      // the type	: 1 int
            + 4 * 8                // bounding box : 4 doubles
            + 4                    // numparts	: 1 int
            + 4                    // numpoints	: 1 int
            + NumParts() * 4       // parts	: np ints
            + NumPoints() * 2 * 8  // points	: 2n doubles
    );
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Write the element
// ----------------------------------------------------------------------

std::ostream& NFmiEsriPolygon::Write(ostream& os) const
{
  try
  {
    os << LittleEndianInt(Type()) << LittleEndianDouble(Box().Xmin())
       << LittleEndianDouble(Box().Ymin()) << LittleEndianDouble(Box().Xmax())
       << LittleEndianDouble(Box().Ymax()) << LittleEndianInt(NumParts())
       << LittleEndianInt(NumPoints());

    int i = 0;
    for (i = 0; i < NumParts(); i++)
      os << LittleEndianInt(Parts()[i]);

    for (i = 0; i < NumPoints(); i++)
    {
      os << LittleEndianDouble(Points()[i].X()) << LittleEndianDouble(Points()[i].Y());
    }

    return os;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Projecting the data
// ----------------------------------------------------------------------

void NFmiEsriPolygon::Project(const NFmiEsriProjector& theProjector)
{
  try
  {
    theProjector.SetBox(itsBox);

    itsBox.Init();
    for (int i = 0; i < NumPoints(); i++)
    {
      NFmiEsriPoint tmp(itsPoints[i].X(), itsPoints[i].Y());
      tmp = theProjector(tmp);
      itsPoints[i].X(tmp.X());
      itsPoints[i].Y(tmp.Y());
      itsBox.Update(tmp.X(), tmp.Y());
    }
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

}  // namespace Imagine

// ======================================================================
