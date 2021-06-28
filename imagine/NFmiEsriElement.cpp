// ======================================================================
//
// Abstract base class for shapefile ESRI element types
// and their attributes.
//
// ======================================================================

#include "NFmiEsriElement.h"
#include <macgyver/Exception.h>

using namespace std;

namespace Imagine
{
// ----------------------------------------------------------------------
// Assignment operator
// ----------------------------------------------------------------------

NFmiEsriElement& NFmiEsriElement::operator=(const NFmiEsriElement& theElement)
{
  try
  {
  if (this != &theElement)
  {
    itsType = theElement.itsType;
    itsNumber = theElement.itsNumber;
    itsAttributes = theElement.itsAttributes;
  }
  return *this;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Return type of attribute
// ----------------------------------------------------------------------

NFmiEsriAttributeType NFmiEsriElement::GetType(const string& theName) const
{
  try
  {
  list<NFmiEsriAttribute>::const_iterator begin = itsAttributes.begin();
  list<NFmiEsriAttribute>::const_iterator end = itsAttributes.end();
  list<NFmiEsriAttribute>::const_iterator iter;

  for (iter = begin; iter != end; ++iter)
  {
    if ((*iter).GetName() == theName)
      return (*iter).GetType();
  }

  // Just some default value, maybe we should throw?

  return kFmiEsriString;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Return string value of attribute
// ----------------------------------------------------------------------

const std::string NFmiEsriElement::GetString(const string& theName) const
{
  try
  {
  list<NFmiEsriAttribute>::const_iterator begin = itsAttributes.begin();
  list<NFmiEsriAttribute>::const_iterator end = itsAttributes.end();
  list<NFmiEsriAttribute>::const_iterator iter;

  for (iter = begin; iter != end; ++iter)
  {
    if ((*iter).GetName() == theName)
      if ((*iter).GetType() == kFmiEsriString)
        return (*iter).GetString();
  }

  // Return empty string if field not found

  static const string tmp = "";
  return tmp;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Return date value of attribute
// ----------------------------------------------------------------------

const NFmiMetTime& NFmiEsriElement::GetDate(const std::string& theName) const
{
  try
  {
  list<NFmiEsriAttribute>::const_iterator begin = itsAttributes.begin();
  list<NFmiEsriAttribute>::const_iterator end = itsAttributes.end();
  list<NFmiEsriAttribute>::const_iterator iter;

  for (iter = begin; iter != end; ++iter)
  {
    if ((*iter).GetName() == theName)
      if ((*iter).GetType() == kFmiEsriDate)
        return (*iter).GetDate();
  }

    throw Fmi::Exception(BCP,"Date field " + theName + " not found");
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Return integer value of attribute
// ----------------------------------------------------------------------

int NFmiEsriElement::GetInteger(const string& theName) const
{
  try
  {
  list<NFmiEsriAttribute>::const_iterator begin = itsAttributes.begin();
  list<NFmiEsriAttribute>::const_iterator end = itsAttributes.end();
  list<NFmiEsriAttribute>::const_iterator iter;

  for (iter = begin; iter != end; ++iter)
  {
    if ((*iter).GetName() == theName)
      if ((*iter).GetType() == kFmiEsriInteger)
        return (*iter).GetInteger();
  }

  // Maybe should error instead..

  return 0;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Return double value of attribute
// ----------------------------------------------------------------------

double NFmiEsriElement::GetDouble(const string& theName) const
{
  try
  {
  list<NFmiEsriAttribute>::const_iterator begin = itsAttributes.begin();
  list<NFmiEsriAttribute>::const_iterator end = itsAttributes.end();
  list<NFmiEsriAttribute>::const_iterator iter;

  for (iter = begin; iter != end; ++iter)
  {
    if ((*iter).GetName() == theName)
      if ((*iter).GetType() == kFmiEsriDouble)
        return (*iter).GetDouble();
  }

  // Maybe should error instead..

  return 0.0;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

}  // namespace Imagine

std::ostream& operator<<(std::ostream& os, const Imagine::NFmiEsriElement& theElement)
{
  try
  {
  return theElement.Write(os);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
