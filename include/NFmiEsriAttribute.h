// ======================================================================
//
// A class to contain a single typed attribute
//
// Constructors:
//
//	NFmiEsriAttribute variable(int|double|string,*NFmiEsriAttributeName);
//
// Getting values:
//
//	variable.GetName();
//	variable.GetType();
//	variable.GetInteger();
//	variable.GetDouble();
//	variable.GetString();
//
// Setting values:
//
//	variable.SetInteger(int);
//	variable.SetDouble(double);
//	variable.SetString(string);
//
// Note that there is no
//
//	variable.Set(int|double|string);
//
// due to possible accidental automatic conversion problems
// between integers and floating point numbers.
//
// Also, the point of having a pointer to NFmiEsriAttributeName is to
// have a single copy of those objects, which is common to all Elements
// in the shape. This will save a lot of memory and is faster to handle.
// 
// History:
//
// 30.08.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================

#ifndef _NFMIESRIATTRIBUTE_H
#define _NFMIESRIATTRIBUTE_H

#include <string>

#include "NFmiEsriAttributeName.h"

class NFmiEsriAttribute
{
public:

  // Constructors based on data types

  NFmiEsriAttribute(int theValue, NFmiEsriAttributeName * theName)
    : itsAttributeName(theName)
    , itsInteger(theValue)
  {}

  NFmiEsriAttribute(double theValue, NFmiEsriAttributeName * theName)
    : itsAttributeName(theName)
    , itsDouble(theValue)
  {}

  NFmiEsriAttribute(const std::string & theValue, NFmiEsriAttributeName * theName)
    : itsAttributeName(theName)
    , itsString(theValue)
  {}

  // Destructor is automatic

  ~NFmiEsriAttribute(void) {}

  // Getting values - it is assumed that type has been checked

  const int GetInteger(void) const			{ return itsInteger; }
  const double GetDouble(void) const			{ return itsDouble; }
  const std::string & GetString(void) const			{ return itsString; }

  const std::string & GetName(void) const
  { return itsAttributeName->Name(); }

  const NFmiEsriAttributeType GetType(void) const
  { return itsAttributeName->Type(); }

  // Setting values - it is assumed that type has been checked

  void SetInteger(int theValue)			{ itsInteger = theValue; }
  void SetDouble(double theValue)		{ itsDouble = theValue; }
  void SetString(const std::string & theValue)	{ itsString = theValue; }

private:

  // Must not have a null-attribute - hide the constructor

  NFmiEsriAttribute(void);

  // The data elements
  //
  // The idea is to save space while we can - but unfortunately
  // strings (classes with constructors etc) cannot be included
  // in unions

  NFmiEsriAttributeName * itsAttributeName;
  std::string		itsString;
  union
  {
    int			itsInteger;
    double		itsDouble;
  };

};

#endif // _NFMIESRIATTRIBUTE_H

// ======================================================================
