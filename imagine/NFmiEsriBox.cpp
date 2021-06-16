// ======================================================================
//
// ======================================================================

#include "NFmiEsriBox.h"
#include <macgyver/Exception.h>

#include <algorithm>

namespace Imagine
{
// ----------------------------------------------------------------------
/*!
 * \brief Void constructor
 */
// ----------------------------------------------------------------------

NFmiEsriBox::NFmiEsriBox(void)
    : itsValidity(false),
      itsXmin(0.0),
      itsXmax(0.0),
      itsYmin(0.0),
      itsYmax(0.0),
      itsMmin(0.0),
      itsMmax(0.0),
      itsZmin(0.0),
      itsZmax(0.0)
{
}

// ----------------------------------------------------------------------
/*!
 * \brief Initialize the bounding box
 */
// ----------------------------------------------------------------------

void NFmiEsriBox::Init(void)
{
  try
  {
    itsXmin = 0.0;
    itsXmax = 0.0;
    itsYmin = 0.0;
    itsYmax = 0.0;
    itsMmin = 0.0;
    itsMmax = 0.0;
    itsZmin = 0.0;
    itsZmax = 0.0;
    itsValidity = false;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Update
 */
// ----------------------------------------------------------------------

void NFmiEsriBox::Update(double theX, double theY, double theM, double theZ)
{
  try
  {
    if (itsValidity)
    {
      itsXmin = std::min(itsXmin, theX);
      itsXmax = std::max(itsXmax, theX);
      itsYmin = std::min(itsYmin, theY);
      itsYmax = std::max(itsYmax, theY);
      itsMmin = std::min(itsMmin, theM);
      itsMmax = std::max(itsMmax, theM);
      itsZmin = std::min(itsZmin, theZ);
      itsZmax = std::max(itsZmax, theZ);
    }
    else
    {
      itsXmin = itsXmax = theX;
      itsYmin = itsYmax = theY;
      itsMmin = itsMmax = theM;
      itsZmin = itsZmax = theZ;
      itsValidity = true;
    }
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Update
 */
// ----------------------------------------------------------------------

void NFmiEsriBox::Update(double theX, double theY, double theM)
{
  try
  {
    if (itsValidity)
    {
      itsXmin = std::min(itsXmin, theX);
      itsXmax = std::max(itsXmax, theX);
      itsYmin = std::min(itsYmin, theY);
      itsYmax = std::max(itsYmax, theY);
      itsMmin = std::min(itsMmin, theM);
      itsMmax = std::max(itsMmax, theM);
    }
    else
    {
      itsXmin = itsXmax = theX;
      itsYmin = itsYmax = theY;
      itsMmin = itsMmax = theM;
      itsValidity = true;
    }
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Update
 */
// ----------------------------------------------------------------------

void NFmiEsriBox::Update(double theX, double theY)
{
  try
  {
    if (itsValidity)
    {
      itsXmin = std::min(itsXmin, theX);
      itsXmax = std::max(itsXmax, theX);
      itsYmin = std::min(itsYmin, theY);
      itsYmax = std::max(itsYmax, theY);
    }
    else
    {
      itsXmin = itsXmax = theX;
      itsYmin = itsYmax = theY;
      itsValidity = true;
    }
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Update
 */
// ----------------------------------------------------------------------

void NFmiEsriBox::Update(const NFmiEsriBox& theBox)
{
  try
  {
    if (theBox.IsValid())
    {
      Update(theBox.Xmin(), theBox.Ymin(), theBox.Mmin(), theBox.Zmin());
      Update(theBox.Xmax(), theBox.Ymax(), theBox.Mmax(), theBox.Zmax());
    }
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

}  // namespace Imagine

// ======================================================================
