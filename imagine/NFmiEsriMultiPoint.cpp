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
#include "NFmiEsriBuffer.h"
#include <macgyver/Exception.h>

using namespace Imagine::NFmiEsriBuffer;  // Conversion tools
using namespace std;

namespace Imagine
{
// ----------------------------------------------------------------------
// Copy constructor
// ----------------------------------------------------------------------

NFmiEsriMultiPoint::NFmiEsriMultiPoint(const NFmiEsriMultiPoint& thePoints)
    : NFmiEsriElement(thePoints), itsBox(thePoints.itsBox), itsPoints(thePoints.itsPoints)
{
}

// ----------------------------------------------------------------------
// Assignment operator
// ----------------------------------------------------------------------

NFmiEsriMultiPoint& NFmiEsriMultiPoint::operator=(const NFmiEsriMultiPoint& thePoints)
{
  try
  {
    if (this != &thePoints)
    {
      NFmiEsriElement::operator=(thePoints);
      itsBox = thePoints.itsBox;
      itsPoints = thePoints.itsPoints;
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

NFmiEsriElement* NFmiEsriMultiPoint::Clone() const
{
  try
  {
    return new NFmiEsriMultiPoint(*this);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Constructor based on a character buffer
// ----------------------------------------------------------------------

NFmiEsriMultiPoint::NFmiEsriMultiPoint(const string& theBuffer, int thePos, int theNumber)
    : NFmiEsriElement(kFmiEsriMultiPoint, theNumber), itsBox(), itsPoints()
{
  try
  {
    int npoints = LittleEndianInt(theBuffer, thePos + 36);

    // Speed up by reserving enough space already

    itsPoints.reserve(itsPoints.size() + npoints);

    for (int i = 0; i < npoints; i++)
    {
      // Start position 40, then 2 doubles (16) for each point
      int pointpos = thePos + 40 + i * 16;
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

int NFmiEsriMultiPoint::StringSize(void) const
{
  try
  {
    return (4                      // the type	: 1 int
            + 4 * 8                // bounding box : 4 doubles
            + 4                    // numpoints	: 1 int
            + NumPoints() * 2 * 8  // points	: 2n doubles
    );
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Writing element
// ----------------------------------------------------------------------

std::ostream& NFmiEsriMultiPoint::Write(ostream& os) const
{
  try
  {
    os << LittleEndianInt(Type()) << LittleEndianDouble(Box().Xmin())
       << LittleEndianDouble(Box().Ymin()) << LittleEndianDouble(Box().Xmax())
       << LittleEndianDouble(Box().Ymax()) << LittleEndianInt(NumPoints());

    for (int i = 0; i < NumPoints(); i++)
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

void NFmiEsriMultiPoint::Project(const NFmiEsriProjector& theProjector)
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
