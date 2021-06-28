/*!
 * \file NFmiAlignment.cpp
 *
 * \b History:
 *
 * \li 30.09.2001, Mika Heiskanen\par
 * Implemented
 *
 */

#include "NFmiAlignment.h"
#include <macgyver/Exception.h>

using namespace std;

namespace Imagine
{
/*!
 * Convert an alignment name to a alignment enum.
 * Returns kFmiAlignmentMissing for an unrecognized name.
 */

NFmiAlignment AlignmentValue(const string& theName)
{
  try
  {
    if (theName == "Center")
      return kFmiAlignCenter;
    if (theName == "NorthWest")
      return kFmiAlignNorthWest;
    if (theName == "North")
      return kFmiAlignNorth;
    if (theName == "NortEast")
      return kFmiAlignNorthEast;
    if (theName == "East")
      return kFmiAlignEast;
    if (theName == "SouthEast")
      return kFmiAlignSouthEast;
    if (theName == "South")
      return kFmiAlignSouth;
    if (theName == "SouthWest")
      return kFmiAlignSouthWest;
    if (theName == "West")
      return kFmiAlignWest;

    return kFmiAlignMissing;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

/*!
 * Convert an alignment enum to a alignment name.
 * Returns a "Unknown" if the name is unknown.
 */

const string AlignmentName(const NFmiAlignment theAlignment)
{
  try
  {
    switch (theAlignment)
    {
      case kFmiAlignCenter:
        return string("Center");
      case kFmiAlignNorthWest:
        return string("NorthWest");
      case kFmiAlignNorth:
        return string("North");
      case kFmiAlignNorthEast:
        return string("NorthEast");
      case kFmiAlignEast:
        return string("East");
      case kFmiAlignSouthEast:
        return string("SouthEast");
      case kFmiAlignSouth:
        return string("South");
      case kFmiAlignSouthWest:
        return string("SouthWest");
      case kFmiAlignWest:
        return string("West");
      default:
        return string("Unknown");
    }
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

/*!
 * Return the X-position in range 0-1 for the given alignment.
 * The value can then be used in linear interpolation to weight
 * original values as in formula
 * \f$x' = \lambda x_1 + (1-\lambda) x_2\f$
 * where \f$\lambda\f$ is the alignment factor.
 */

double XAlignmentFactor(NFmiAlignment theAlignment)
{
  try
  {
    switch (theAlignment)
    {
      case kFmiAlignNorthEast:
      case kFmiAlignEast:
      case kFmiAlignSouthEast:
        return 1.0;
      case kFmiAlignCenter:
      case kFmiAlignNorth:
      case kFmiAlignSouth:
        return 0.5;
      default:
        return 0.0;
    }
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

/*!
 * Return the Y-position in range 0-1 for the given alignment.
 * The value can then be used in linear interpolation to weight
 * original values as in formula
 * \f$y' = \lambda y_1 + (1-\lambda) y_2\f$
 * where \f$\lambda\f$ is the alignment factor.
 */

double YAlignmentFactor(NFmiAlignment theAlignment)
{
  try
  {
    switch (theAlignment)
    {
      case kFmiAlignSouthWest:
      case kFmiAlignSouthEast:
      case kFmiAlignSouth:
        return 1.0;
      case kFmiAlignEast:
      case kFmiAlignCenter:
      case kFmiAlignWest:
        return 0.5;
      default:
        return 0.0;
    }
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

}  // namespace Imagine

// ======================================================================
